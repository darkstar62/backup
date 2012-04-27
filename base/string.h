// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#ifndef BACKUP_BASE_STRING_H_
#define BACKUP_BASE_STRING_H_

#include <iostream>
#include <sstream>
#include <string>

template<class T>
const std::string ToString(T& value) {
  std::ostringstream strstr;
  strstr << value;
  return strstr.str();
}

#endif  // BACKUP_BASE_STRING_H_
