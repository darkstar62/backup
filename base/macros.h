// Copyright (C) 2012, All Rights Reserved.
// Author: Cory Maccarrone <darkstar6262@gmail.com>

#ifndef BACKUP_BASE_MACROS_H_
#define BACKUP_BASE_MACROS_H_

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

#endif  // BACKUP_BASE_MACROS_H_
