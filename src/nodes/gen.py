#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
gen.py - generate node .hpp/.cpp for your BaseNodeModel + NodeDesc system
(Enhanced version supporting Categories.h automatic category mapping)

Usage:
  python gen.py params.json
  python gen.py ClassName params.json
"""

import sys, os, json, re

# ============================================================
#  Parse Categories.h
# ============================================================

def load_categories(header_path):
    """
    Parse Categories.h for lines like:
      static const QString CAT_PROC_FILTER = "Processing/Filter";
    """
    categories = {}
    if not os.path.exists(header_path):
        print(f"Warning: Categories.h not found at {header_path}")
        return categories

    # Pattern to match single or multi-line definitions like:
    # static const QString CAT_PROC_FILTER = "Processing/Filter";
    # or
    # static const QString CAT_PROC_FILTER =
    #     "Processing/Filter";
    with open(header_path, "r", encoding="utf8") as f:
        content = f.read()

    # Split by semicolon to handle complete definitions
    definitions = content.split(';')
    
    # Pattern to match: static const QString NAME = "value"
    pattern = re.compile(r'static\s+const\s+QString\s+(\w+)\s*=\s*"([^"]+)"', re.MULTILINE | re.DOTALL)

    for definition in definitions:
        matches = pattern.findall(definition)
        for match in matches:
            key, value = match[0], match[1]
            categories[key] = value

    return categories


# ============================================================
#  Type Mappings
# ============================================================

TYPE_MAP = {
    "int":    ("int",     "ParamDesc::makeInt"),
    "double": ("double",  "ParamDesc::makeDouble"),
    "float":  ("double",  "ParamDesc::makeDouble"),
    "bool":   ("bool",    "ParamDesc::makeBool"),
    "string": ("QString", "ParamDesc::makeString"),
    "enum":   ("int",     "ParamDesc::makeCombo"),  # enum stored as int index
}

PORT_TYPE_MAP = {
    "Mat": "NodeType::Mat",
    "Image": "NodeType::Mat",
    "Points": "NodeType::Points",
    "Number": "NodeType::Number",
    "Contours": "NodeType::Contours",
    "KeyPoints": "NodeType::KeyPoints",
    "Rect": "NodeType::Rect",
    "VisionGeometry": "NodeType::VisionGeometry",
    "AffineMatrix": "NodeType::AffineMatrix"
    # add more mappings if needed...
}


# ============================================================
#  Helpers
# ============================================================

def normalize_class(name: str) -> str:
    s = re.sub(r'[\W_]+', ' ', name).strip()
    if not s:
        return "NewNode"
    parts = s.split()
    cname = "".join(p.capitalize() for p in parts)
    return cname if cname.endswith("Node") else cname + "Node"

def load_json(path_or_str):
    if os.path.exists(path_or_str):
        with open(path_or_str, "r", encoding="utf8") as f:
            return json.load(f)
    try:
        return json.loads(path_or_str)
    except:
        return {}

def build_params(params: dict):
    """
    Return:
        member code
        param desc code
    """
    if not params:
        return "", ""

    members = []
    descs = []

    for name, spec in params.items():
        t = spec.get("type", "int")
        default = spec.get("default", 0)

        # ---------------- enum ----------------
        if t == "enum":
            items = spec.get("items", [])
            if isinstance(default, str) and default in items:
                default_idx = items.index(default)
            else:
                try:
                    default_idx = int(default)
                except:
                    default_idx = 0

            members.append(f"    int _{name} = {default_idx};")

            names_cpp = ", ".join([f'QString("{s}")' for s in items])
            values_cpp = ", ".join([str(i) for i in range(len(items))]) if items else "0"

            descs.append(
                f'        ParamDesc::makeCombo("{name}", "{name}", QVector<QString>{{ {names_cpp} }}, QVector<int>{{ {values_cpp} }}, {default_idx})'
            )
            continue

        # ---------------- normal types ----------------
        if t not in TYPE_MAP:
            t = "string"

        cpp_type, factory = TYPE_MAP[t]

        # member defaults
        if t == "string":
            members.append(f'    {cpp_type} _{name} = QString("{default}");')
            desc_line = f'        {factory}("{name}", "{name}", QString("{default}"))'
            descs.append(desc_line)

        elif t == "bool":
            default_literal = "true" if default else "false"
            members.append(f"    bool _{name} = {default_literal};")
            descs.append(f'        {factory}("{name}", "{name}", {default_literal})')

        else:
            # int/double
            members.append(f"    {cpp_type} _{name} = {default};")
            line = f'        {factory}("{name}", "{name}", {default})'

            if "min" in spec and "max" in spec:
                line += f'.setRange({spec["min"]}, {spec["max"]})'
            if "step" in spec:
                line += f'.setStep({spec["step"]})'

            descs.append(line)

    return "\n".join(members), ",\n".join(descs)

def build_ports(port_list, is_input=True):
    if not port_list:
        return ""
    lines = []
    for p in list(port_list):
        pname = p.get("name", "")
        ptype = p.get("type", "Mat")
        node_type = PORT_TYPE_MAP.get(ptype, "NodeType::Mat")
        if is_input:
            lines.append(f'        PortDesc::In("{pname}", {node_type})')
        else:
            lines.append(f'        PortDesc::Out("{pname}", {node_type})')
    return ",\n".join(lines)


# ============================================================
#  Templates
# ============================================================

HPP_TMPL = r'''#pragma once
#include "processing.h"

class {CLASS} : public BaseNodeModel
{{
    Q_OBJECT
public:
    {CLASS}();
    static const NodeDesc desc;
    ~{CLASS}() override = default;

protected:
    void process() override;

private:
    cv::Mat _input;
    cv::Mat _output;

{MEMBERS}
}};
'''

CPP_TMPL = r'''#include "{CLASS}.hpp"

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


def get_unique_filename(filepath):
    """Generate a unique filename by adding a counter if the file exists"""
    if not os.path.exists(filepath):
        return filepath
    
    base, ext = os.path.splitext(filepath)
    counter = 1
    while True:
        new_filepath = f"{base}-{counter}{ext}"
        if not os.path.exists(new_filepath):
            return new_filepath
        counter += 1


def update_register_file(register_file_path, category, class_name):
    """Update the register_image_nodes.cpp file to include the new node"""
    if not os.path.exists(register_file_path):
        print(f"Warning: Register file not found at {register_file_path}, skipping update.")
        return False

    with open(register_file_path, 'r', encoding='utf8') as f:
        lines = f.readlines()  # Read into a list of lines for easier manipulation

    # Determine the section for the node based on category
    section_map = {
        "Processing/Filter": "filter",
        "Processing/BasicOps": "basic ops",
        "Processing/Geometry": "geometry", 
        "Processing/Morphology": "morph",
        "Processing/Color": "basic ops",  # Color ops are usually basic ops
        "Processing/IO": "IO",
        "Processing/Drawing": "draw",
        "Analytics/Contours": "basic ops",  # Contours are basic processing
        "Analytics/Thresholding": "basic ops",  # Thresholding is basic processing
    }

    # Determine category section
    section = section_map.get(category, "basic ops")  # Default to basic ops
    
    # Create the include statement
    dir_parts = category.lower().replace('\\', '/').split('/')
    subdir = dir_parts[1] if len(dir_parts) >= 2 else (dir_parts[0] if dir_parts[0] != "Processing" else "misc")
    include_stmt = f'#include "{subdir}/{class_name}.hpp"'

    # --- Update Includes ---
    # Check if include statement already exists
    if any(include_stmt in line for line in lines):
        print(f"Include statement for {class_name} already exists, skipping include update.")
    else:
        # Find all includes for the same subdirectory
        subdir_include_indices = [i for i, line in enumerate(lines) if f'"{subdir}/' in line and line.strip().startswith('#include')]
        
        if subdir_include_indices:
            # Find the correct alphabetical position within the existing block
            insert_pos = None
            for idx in subdir_include_indices:
                if lines[idx].strip() > include_stmt:
                    insert_pos = idx
                    break
            insert_pos = insert_pos if insert_pos is not None else subdir_include_indices[-1] + 1
        else:
            # No includes for this subdir exist; find the last #include line before the function body
            last_include_idx = -1
            for i, line in enumerate(lines):
                if line.strip().startswith('#include'):
                    last_include_idx = i
                elif line.strip().startswith('extern "C"'):
                    break
            insert_pos = last_include_idx + 1 if last_include_idx != -1 else 0

        # Insert the new include statement
        lines.insert(insert_pos, include_stmt + '\n')

    # --- Update Registration Calls ---
    register_call = f"    REGISTER_NODE_IN_PLUGIN({class_name});"
    
    # Check if register call already exists
    if any(register_call in line for line in lines):
        print(f"Register call for {class_name} already exists, skipping register update.")
    else:
        # Find the target section and insert alphabetically
        in_target_section = False
        section_start_idx = -1
        section_end_idx = len(lines)

        # Locate the boundaries of the target section
        for i, line in enumerate(lines):
            if f"//{section}" in line:
                in_target_section = True
                section_start_idx = i + 1
            elif in_target_section and line.strip().startswith('//'):
                section_end_idx = i
                break
            elif in_target_section and 'return true;' in line:
                section_end_idx = i
                break

        if in_target_section:
            # Find the correct alphabetical position within the section for registration calls
            insert_pos = section_end_idx
            for i in range(section_start_idx, section_end_idx):
                if 'REGISTER_NODE_IN_PLUGIN(' in lines[i] and lines[i].strip() > register_call:
                    insert_pos = i
                    break
            lines.insert(insert_pos, register_call + '\n')
        else:
            # Fallback: Append before the return statement
            for i, line in enumerate(reversed(lines)):
                if 'return true;' in line:
                    lines.insert(len(lines) - i, register_call + '\n')
                    break

    # Write the updated content back to the file
    with open(register_file_path, 'w', encoding='utf8') as f:
        f.writelines(lines)
    
    print(f"Updated {register_file_path} with {class_name}")
    return True


# ============================================================
#  Main
# ============================================================

def main():
    if len(sys.argv) < 2:
        print("Usage: python gen.py params.json")
        print("       python gen.py ClassName params.json")
        return

    # determine json + class name
    if len(sys.argv) == 2:
        json_arg = sys.argv[1]
        cfg = load_json(json_arg)

        if not cfg:
            print("Error: Invalid JSON or file not found.")
            return

        class_name = cfg.get("class_name")
        if not class_name:
            if os.path.exists(json_arg):
                base = os.path.splitext(os.path.basename(json_arg))[0]
                class_name = normalize_class(base)
            else:
                class_name = "NewNode"
    else:
        class_name = normalize_class(sys.argv[1])
        json_arg = sys.argv[2]
        cfg = load_json(json_arg)

    # -------------------------------------------------------------------
    # Load Categories.h
    # -------------------------------------------------------------------
    script_dir = os.path.dirname(os.path.abspath(__file__))
    # Path to Categories.h from the script location - should go up one level to reach src/, then into include/
    cat_header = os.path.join(script_dir, "..", "include", "Categories.h")
    CATEGORY_MAP = load_categories(cat_header)

    raw_category = cfg.get("category", "General")

    # If category is a constant like CAT_PROC_FILTER, replace with its value
    if raw_category in CATEGORY_MAP:
        category = CATEGORY_MAP[raw_category]  # e.g., "Processing/Filter" 
    else:
        category = raw_category  # allow direct path like "Processing/Filter"

    # -------------------------------------------------------------------
    # Extract I/O & params
    # -------------------------------------------------------------------
    caption = cfg.get("caption", class_name)
    params = cfg.get("params", {})
    inputs = cfg.get("inputs", [])
    outputs = cfg.get("outputs", [])

    members_code, params_code = build_params(params)
    inputs_code, outputs_code = build_ports(inputs, True), build_ports(outputs, False)

    # -------------------------------------------------------------------
    # Output paths - split category path for proper structure
    # -------------------------------------------------------------------
    # category: "Processing/Filter" -> nodes/Processing/include/Filter/ and nodes/Processing/src/Filter/
    category_parts = category.replace("\\", "/").split("/")
    main_dir = category_parts[0]  # e.g., "Processing"
    sub_dirs = "/".join(category_parts[1:]) if len(category_parts) > 1 else ""  # e.g., "Filter"

    project_root = os.path.abspath(os.path.join(script_dir))
    
    # Structure: nodes/main_dir/include/sub_dirs/ and nodes/main_dir/src/sub_dirs/
    include_base = os.path.join(project_root, main_dir, "include")
    src_base = os.path.join(project_root, main_dir, "src")
    
    if sub_dirs:
        include_dir = os.path.join(include_base, sub_dirs)
        src_dir = os.path.join(src_base, sub_dirs)
    else:
        include_dir = include_base
        src_dir = src_base

    os.makedirs(include_dir, exist_ok=True)
    os.makedirs(src_dir, exist_ok=True)

    hpp_path = os.path.normpath(os.path.join(include_dir, f"{class_name}.hpp"))
    cpp_path = os.path.normpath(os.path.join(src_dir, f"{class_name}.cpp"))

    # Get unique filenames to avoid overwriting
    hpp_path = get_unique_filename(hpp_path)
    cpp_path = get_unique_filename(cpp_path)

    # -------------------------------------------------------------------
    # Generate C++
    # -------------------------------------------------------------------
    hpp = HPP_TMPL.format(CLASS=class_name, MEMBERS=members_code)
    cpp = CPP_TMPL.format(
        CLASS=class_name,
        NAME=class_name,
        DISPLAY=caption,
        CATEGORY=category,
        INPUTS=inputs_code if inputs_code else "        ",
        OUTPUTS=outputs_code if outputs_code else "        ",
        PARAMS=params_code if params_code else "        "
    )

    with open(hpp_path, "w", encoding="utf8") as f:
        f.write(hpp)

    with open(cpp_path, "w", encoding="utf8") as f:
        f.write(cpp)

    # -------------------------------------------------------------------
    # Update register file
    # -------------------------------------------------------------------
    register_file_path = os.path.join(project_root, main_dir.lower(), "src", "register_image_nodes.cpp")
    update_register_file(register_file_path, category, class_name)

    print("Generated:")
    print("   ", hpp_path)
    print("   ", cpp_path)


if __name__ == "__main__":
    main()