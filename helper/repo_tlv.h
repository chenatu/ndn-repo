/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */
#ifndef REPO_TLV_H
#define REPO_TLV_H

namespace tlv_repo{
enum {
  // ControlResponse
  RepoCommandParameter = 201,
  Name = 7, // Name
  Selectors = 9,
  StartBlockId = 204,
  EndBlockId = 205,
  ProcessId =206,
  RepoCommandResponse = 207,
  StatusCode = 208,
  InsertNum = 209,
  DeleteNum = 210
};
} //tlv_repo

#endif