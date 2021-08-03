#include <exception>

#ifndef SAPEAOB_ERRORS_
#define SAPEAOB_ERRORS_

namespace sapeaob {

struct pattern_not_found : public std::exception {
  const char *what() const throw() { return "Pattern couldn't be found"; }
};

struct size_error : public std::exception {
  const char *what() const throw() { return "Missmatch of size operands"; }
};

} // namespace sapeaob

#endif
