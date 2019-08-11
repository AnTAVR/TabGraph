/*
* @Author: gpinchon
* @Date:   2019-08-10 11:20:21
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-10 20:24:12
*/

#pragma once

#include "parser/FBX/FBXObject.hpp"
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace FBX {

struct Node : public Object {
    static std::shared_ptr<Node> Create();
    /** @return a vector containing all nodes with this name */
    std::vector<std::shared_ptr<Node>>& SubNodes(const std::string& name);
    /** @return the first node named with this name */
    std::shared_ptr<Node> SubNode(const std::string& name);
    /** @return the property at this index */
    class Property& Property(const size_t index);
    virtual void Print() const;
    std::vector<std::shared_ptr<class Property>> properties;
    std::map<std::string, std::vector<std::shared_ptr<Node>>> nodes;

protected:
    Node() = default;
};
}
