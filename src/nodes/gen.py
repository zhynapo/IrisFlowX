#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
gen.py - generate node .hpp/.cpp for your BaseNodeModel + NodeDesc system

Usage:
  python gen.py params.json
  python gen.py ClassName params.json

JSON format example:
{
  "class_name": "GaussianBlur",
  "caption": "Gaussian Blur",
  "category": "Filter",
  "inputs": [
    {"name":"image","type":"Mat"}
  ],
  "outputs": [
    {"name":"image","type":"Mat"}
  ],
  "params": {
    "ksize": {"type":"int","default":3, "min":1, "max":31, "step":2},
    "sigmaX":{"type":"double","default":1.0, "min":0.0, "max":5.0},
    "mode": {"type":"enum","items":["nearest","linear","cubic"], "default":"linear"}
  }
}
"""

import sys, os, json, re

# Map JSON param types -> (C++ type, ParamDesc factory)
TYPE_MAP = {
    "int":    ("int",     "ParamDesc::makeInt"),
    "double": ("double",  "ParamDesc::makeDouble"),
    "float":  ("double",  "ParamDesc::makeDouble"),
    "bool":   ("bool",    "ParamDesc::makeBool"),
    "string": ("QString", "ParamDesc::makeString"),
    "enum":   ("int",     "ParamDesc::makeCombo"),  # enum stored as int index
}

# Map port type string -> NodeType symbol used in PortDesc
PORT_TYPE_MAP = {
    "Mat": "NodeType::Mat",
    "Image": "NodeType::Mat",
    "Points": "NodeType::Points",
    "Number": "NodeType::Number",
    "Contours": "NodeType::Contours",
    # add more mappings as needed
}

# ---------------- utilities ----------------
def normalize_class(name: str) -> str:
    s = re.sub(r'[\W_]+', ' ', name).strip()
    if not s:
        return "NewNode"
    parts = s.split()
    cname = "".join(p.capitalize() for p in parts)
    return cname if cname.endswith("Node") else cname + "Node"

def normalize_category(name: str) -> str:
    if not name:
        return "general"
    # simple normalization: lower, replace non-word by underscore
    s = re.sub(r'[\W_]+', '_', name.strip()).strip('_').lower()
    return s or "general"

def load_json(path_or_str):
    if not path_or_str:
        return {}
    # if it's a path to file
    if os.path.exists(path_or_str):
        with open(path_or_str, "r", encoding="utf8") as f:
            return json.load(f)
    # try parse as json string
    try:
        return json.loads(path_or_str)
    except Exception:
        return {}

def cpp_literal(v):
    if isinstance(v, str):
        return f'QString("{v}")'  # use QString literal for strings in members; ParamDesc factories expect raw literals too sometimes
    if isinstance(v, bool):
        return "true" if v else "false"
    # numbers or others
    return str(v)

# ---------------- build params ----------------
def build_params(params: dict):
    """
    returns (members_code, paramdesc_code)
    members_code: lines to insert into class for param members
    paramdesc_code: lines inside NodeDesc parameter list
    """
    if not params:
        return "", ""

    members = []
    descs = []

    for name, spec in params.items():
        t = spec.get("type", "int")
        default = spec.get("default", 0)

        if t == "enum":
            items = spec.get("items", [])
            # default index
            if isinstance(default, str) and default in items:
                default_idx = items.index(default)
            else:
                try:
                    default_idx = int(default)
                except:
                    default_idx = 0
            members.append(f"    int _{name} = {default_idx};")
            # build QVector<QString>{ QString("a"), ... }
            names_cpp = ", ".join([f'QString("{s}")' for s in items])
            values_cpp = ", ".join([str(i) for i in range(len(items))]) if items else "0"
            descs.append(
                f'        ParamDesc::makeCombo("{name}", "{name}", QVector<QString>{{ {names_cpp} }}, QVector<int>{{ {values_cpp} }}, {default_idx})'
            )
            continue

        # normal types
        if t not in TYPE_MAP:
            # fallback to string
            t = "string"
        cpp_type, factory = TYPE_MAP[t]

        # prepare member default literal (use simple forms)
        if t == "string":
            member_default = f'QString("{default}")'
            members.append(f"    {cpp_type} _{name} = {member_default};")
            desc_line = f'        {factory}("{name}", "{name}", QString("{default}"))'
            descs.append(desc_line)
        elif t == "bool":
            member_default = "true" if default else "false"
            members.append(f"    {cpp_type} _{name} = {member_default};")
            descs.append(f'        {factory}("{name}", "{name}", {member_default})')
        else:
            # int/double
            members.append(f"    {cpp_type} _{name} = {default};")
            line = f'        {factory}("{name}", "{name}", {default})'
            # optional range/step
            if "min" in spec and "max" in spec:
                line += f'.setRange({spec["min"]}, {spec["max"]})'
            if "step" in spec:
                line += f'.setStep({spec["step"]})'
            descs.append(line)

    return "\n".join(members), ",\n".join(descs)

# ---------------- build ports ----------------
def build_ports(port_list, is_input=True):
    """
    port_list: list of {"name":..., "type":...} where type is logical name like "Mat","Points","Number"
    returns C++ block lines like: PortDesc::In("image", NodeType::Mat), ...
    """
    if not port_list:
        return ""
    lines = []
    for p in port_list:
        pname = p.get("name", "")
        ptype = p.get("type", "Mat")
        node_type = PORT_TYPE_MAP.get(ptype, "NodeType::Mat")
        # choose In/Out variant
        if is_input:
            lines.append(f'        PortDesc::In("{pname}", {node_type})')
        else:
            lines.append(f'        PortDesc::Out("{pname}", {node_type})')
    return ",\n".join(lines)

# ---------------- templates (double braces for literal braces) ----------------
HPP_TMPL = r'''#pragma once
#include "model/BaseNodeModel.hpp"
#include "base/MatNodeData.hpp"
#include <opencv2/opencv.hpp>

using namespace Flow;

class {CLASS} : public BaseNodeModel
{{
    Q_OBJECT
public:
    {CLASS}();
    static const NodeDesc desc;
    ~{CLASS}() override = default;

    // ---- Processing ----
protected:
    void process() override;

private:
    cv::Mat _input;
    cv::Mat _output;

{MEMBERS}
}};
'''

CPP_TMPL = r'''#include "{CLASS}.hpp"
#include "Nodetype.hpp"

const NodeDesc {CLASS}::desc =
{{
    "{NAME}",
    "{DISPLAY}",
    "{CATEGORY}",

    // inputs
    {{
{INPUTS}
    }},

    // outputs
    {{
{OUTPUTS}
    }},

    // parameters
    {{
{PARAMS}
    }}
}};

{CLASS}::{CLASS}()
    : BaseNodeModel(desc)
{{
}}

void {CLASS}::process()
{{
    auto matData = std::dynamic_pointer_cast<MatNodeData>(_getInput(0));
    if (!matData)
    {{
        setOutputData(0, nullptr);
        return;
    }}

    _input = matData->mat();

    // TODO: implement algorithm
    _output = _input.clone();

    setOutputData(0, std::make_shared<MatNodeData>(_output));
}}
'''

# ---------------- main ----------------
def main():
    if len(sys.argv) < 2:
        print("Usage:")
        print("  python gen.py params.json")
        print("  python gen.py ClassName params.json")
        return

    if len(sys.argv) == 2:
        json_arg = sys.argv[1]
        cfg = load_json(json_arg)
        # if cfg empty and the arg looks like JSON string, try parse already done
        # determine class name
        class_name = cfg.get("class_name") if isinstance(cfg, dict) and cfg.get("class_name") else None
        if not class_name:
            # derive from filename if file path
            if os.path.exists(json_arg):
                base = os.path.splitext(os.path.basename(json_arg))[0]
                class_name = normalize_class(base)
            else:
                class_name = "NewNode"
    else:
        class_name = normalize_class(sys.argv[1])
        json_arg = sys.argv[2]
        cfg = load_json(json_arg)

    # allow root aliasing: if top-level is params only, support both
    caption = cfg.get("caption", class_name)
    category = cfg.get("category", "General")
    # support either "params" object or direct param dict
    params = cfg.get("params", None)
    if params is None:
        # maybe the file itself is params dict
        # detect typical keys (first-level keys with type)
        # if "inputs" in cfg then treat params inside params
        if "inputs" in cfg or "outputs" in cfg or "caption" in cfg:
            params = cfg.get("params", {})
        else:
            params = cfg

    inputs = cfg.get("inputs", [])
    outputs = cfg.get("outputs", [])

    members_code, params_code = build_params(params)
    inputs_code = build_ports(inputs, is_input=True)
    outputs_code = build_ports(outputs, is_input=False)

    # fill templates
    hpp = HPP_TMPL.format(CLASS=class_name, MEMBERS=members_code)
    cpp = CPP_TMPL.format(CLASS=class_name, NAME=class_name, DISPLAY=caption, CATEGORY=category,
                         INPUTS=inputs_code if inputs_code else "        ",
                         OUTPUTS=outputs_code if outputs_code else "        ",
                         PARAMS=params_code if params_code else "        ")

    # decide output directory: if category provided, generate under src/nodes/<category>/
    script_dir = os.path.dirname(os.path.abspath(__file__))
    cat_dir = normalize_category(category)
    outdir = os.path.join(script_dir, cat_dir)
    os.makedirs(outdir, exist_ok=True)
    # also write a small CMake helper snippet for manual inclusion
    cmake_snippet = os.path.join(outdir, f"{class_name}_CMakeSnippet.txt")
    with open(cmake_snippet, "w", encoding="utf8") as sf:
        sf.write("# CMake snippet: add a shared library target for this node\n")
        sf.write(f"# add_library({class_name} SHARED {class_name}.cpp {class_name}.hpp)\n")
        sf.write("# target_link_libraries(... PRIVATE FlowCore nodes_common_headers)\n")
        sf.write("# Optionally add POST_BUILD copy to ${CMAKE_BINARY_DIR}/bin/${CMAKE_CFG_INTDIR}/plugins/nodes\n")
    hpp_path = os.path.join(outdir, f"{class_name}.hpp")
    cpp_path = os.path.join(outdir, f"{class_name}.cpp")

    with open(hpp_path, "w", encoding="utf8") as f:
        f.write(hpp)
    with open(cpp_path, "w", encoding="utf8") as f:
        f.write(cpp)

    print(f"Generated: {hpp_path} and {cpp_path}")

if __name__ == "__main__":
    main()
