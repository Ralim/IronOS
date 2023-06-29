#!/usr/bin/env python3


"""
Read in the yaml config files in this folder
For each config file, generate a cpp file defining each layout
"""

import yaml
from string import Template


def element_to_str(element) -> str:
    template = Template(
        """
        {
            .elementType = ElementTypes_t::$type,
            .elementSettings={
            .position ={
                .x=$x,
                .y=$y,
                },
            .size ={
                .w=$w,
                .h=$h,
                }
            },
        },
    """
    )
    return template.substitute(
        x=element["position"]["x"],
        y=element["position"]["y"],
        w=element["size"]["w"],
        h=element["size"]["h"],
        type=element["type"],
    )


def layout_to_str(layout):
    inner_elements = "".join(element_to_str(x) for x in layout["elements"])
    template = Template(
        """{
.elements =
    {
        $elements
    },
},
    """
    )
    return template.substitute(elements=inner_elements)


def layouts_to_cpp(layout_file_path, output_file_path):
    with open(layout_file_path, "r") as file:
        with open(output_file_path, "w") as output:
            layout_defs = yaml.safe_load(file)

            header = """
#include "UI.h"
#include "UI_Elements.h"

const ScreenLayoutRecord_t screenLayouts[] = {
"""
            output.write(header)

            for layout_name in layout_defs["layouts"]:
                layout = layout_defs["layouts"][layout_name]
                # Now we convert this layout into a structure definition
                element_entry = layout_to_str(layout)
                output.write(element_entry)
            footer = """
};
"""
            output.write(footer)


layouts_to_cpp("layout_96x16.yaml", "layout_96x16.cpp")
