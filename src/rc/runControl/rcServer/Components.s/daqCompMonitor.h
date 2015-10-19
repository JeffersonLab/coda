//-----------------------------------------------------------------------------
// Copyright (c) 1994,1995 Southeastern Universities Research Association,
//                         Continuous Electron Beam Accelerator Facility
//
// This software was developed under a United States Government license
// described in the NOTICE file included as part of this distribution.
//
// CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
//       coda@cebaf.gov  Tel: (804) 249-7030     Fax: (804) 249-5800
//-----------------------------------------------------------------------------
//
// Description:
//      CODA runControl Server: all components' monitor
//
//       This monitor only starts monitoring all components in the
//       state of active. If it finds any components disconnected,
//       it will end the run and bring every component in the state
//       of configured.

// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqCompMonitor.h,v $
//   Revision 1.2  1996/11/04 16:13:45  chen
//   add options for monitoring components
//
//   Revision 1.1.1.1  1996/10/11 13:39:20  chen
//   run control source
//
//
#ifndef _CODA_DAQCOMP_MONITOR_H
#define _CODA_DAQCOMP_MONITOR_H

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <daqRun.h>
#include <daqCompMonitorTimer.h>
#include <codaSlist.h>

class daqCompMonitor
{
public:
  // constructor and destructor
  daqCompMonitor  (daqRun& run, int timerInterval = 10);
  ~daqCompMonitor (void);

  // start/emd monitor operation
  void startMonitoringComp (void);
  void endMonitoringComp   (void);

  // end transitioner need to find disconnected process
  void enableDisconnectedComp (void);
  
  // set/get timerinterval
  void timerInterval       (int sec);
  int  timerInterval       (void) const;

  // enable/disable auto end
  void enableAutoEnd       (void);
  void disableAutoEnd      (void);
  int  autoend             (void) const;

  // timer callback: polling method
  void timerCallback       (void);

private:

  // daqRun reference
  daqRun& run_;

  // disconnected components are here
  codaSlist junkList_;

  // timer object
  daqCompMonitorTimer timer_;

  // autoend flag
  int autoend_;
};
#endif

  
  
  
