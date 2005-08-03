#ifndef PYASYNCHIO_PYTHONIC_PROACTOR_HPP_INCLUDED_
#define PYASYNCHIO_PYTHONIC_PROACTOR_HPP_INCLUDED_

#pragma once

#include <pyasynchio/detail/config.hpp>
#include <pyasynchio/Proactor.hpp>

namespace pyasynchio {

class PYASYNCHIO_LINK_DECL PythonicProactor : public Proactor
{
public:
    PythonicProactor();
    ~PythonicProactor();
};

}

#endif // PYASYNCHIO_PYTHONIC_PROACTOR_HPP_INCLUDED_