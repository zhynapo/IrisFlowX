#include <QWidget>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QSlider>
#include <QCheckBox>
#include <QLineEdit>
#include <nlohmann/json.hpp>
#include "model/BaseNodeModel.hpp"
#include "IAlg.hpp"
#include "MatNodeData.hpp" // 假设你已定义 cv::Mat 的包装类

class GenericNodeModel : public Flow::BaseNodeModel {
    Q_OBJECT
public:
    GenericNodeModel(std::unique_ptr<IAlg> alg, QString name)
        : _alg(std::move(alg)), _name(name) {
        _widget = new QWidget();
        _layout = new QFormLayout(_widget);
        
        // 1. 核心自动化：解析算法 Schema 并构建 UI
        buildUI();
    }

    // QtNodes 端口定义
    unsigned int nPorts(QtNodes::PortType type) const override { return 1; }
    QtNodes::NodeDataType dataType(QtNodes::PortType, QtNodes::PortIndex) const override {
        return MatNodeData().type(); // 统一使用 Mat 类型
    }

    // 数据输入：当上游数据传来时触发计算
    void setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex) override {
        auto matData = std::dynamic_pointer_cast<MatNodeData>(data);
        if (matData) {
            _inputMat = matData->mat();
            process();
        }
    }

    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override {
        return std::make_shared<MatNodeData>(_outputMat);
    }

    QWidget* embeddedWidget() override { return _widget; }

private:
    void buildUI() {
        auto j = nlohmann::json::parse(_alg->getParamSchema());
        for (auto& item : j["params"]) {
            QString label = QString::fromStdString(item.value("label", ""));
            std::string name = item["name"];
            std::string type = item["type"];

            if (type == "int") {
                auto slider = new QSlider(Qt::Horizontal);
                slider->setRange(item["min"], item["max"]);
                slider->setValue(item["default"]);
                _layout->addRow(label, slider);
                
                // 绑定信号：滑动时自动通知算法
                connect(slider, &QSlider::valueChanged, [this, name](int val) {
                    updateParam(name, val);
                });
            } 
            // 这里可以继续扩展 double (QDoubleSpinBox), string (QLineEdit) 等
        }
    }

    void updateParam(const std::string& name, nlohmann::json value) {
        nlohmann::json j;
        j[name] = value;
        _alg->setParams(j.dump()); // 实时传回算法
        process(); // 触发重绘
    }

    void process() {
        if (_inputMat.empty()) return;
        // 执行算法
        _alg->run(_inputMat.data, _inputMat.cols, _inputMat.rows, _inputMat.channels());
        // 获取结果... (这里可以根据需要实现逻辑)
        emit dataUpdated(0);
    }

    std::unique_ptr<IAlg> _alg;
    QString _name;
    QWidget* _widget;
    QFormLayout* _layout;
    cv::Mat _inputMat, _outputMat;
};