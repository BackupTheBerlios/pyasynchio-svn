#ifndef PYSTE_BOOST_PYTHON_WARNINGS_FIX_HPP_INCLUDED_
#define PYSTE_BOOST_PYTHON_WARNINGS_FIX_HPP_INCLUDED_

#pragma once

#pragma warning(disable:4267)
#include <boost/python/converter/builtin_converters.hpp>
#pragma warning(default:4267)

#endif // PYSTE_BOOST_PYTHON_WARNINGS_FIX_HPP_INCLUDED_