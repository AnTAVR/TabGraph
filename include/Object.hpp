/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 12:56:19
*/

#pragma once

#include <memory> // for enable_shared_from_this, shared_ptr
#include <string> // for string
#include <vector> // for vector

/*
** Never allocate on the stack, always of the heap !!!
** ALWAYS STORE IN A SHARED_PTR !!!
*/

class Object : public std::enable_shared_from_this<Object> {
public:
    int64_t Id() const;
    void SetId(int64_t);
    std::string Name() const;
    void SetName(const std::string& name);
    virtual ~Object();

protected:
    Object() = delete;
    Object(const std::string& name);

private:
    std::string _name { "" };
    int64_t _id { 0 };
};
