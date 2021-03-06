/**********************************************************************
// @@@ START COPYRIGHT @@@
//
// (C) Copyright 1998-2014 Hewlett-Packard Development Company, L.P.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
// @@@ END COPYRIGHT @@@
**********************************************************************/
// Implementation for the classes listed in SystemParameters.h

#define pdctctlz_h_dct_get_by_name_	 // so that we only get dct_get_by_name
#define pdctctlz_h_including_section	 // from pdctctlz.h
#define pdctctlz_h_

//
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//
//                            !!! ATTENTION !!!
//
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//
// For some reason, due (I believe) to strange circular dependencies, by
// adding the compiler directive <#include "NAStringDef.h"> and using
// NAString objects, the linker spits out some extremely bizarre
// (completely inexplicable) errors while building tdm_sqlcli.dll.
//
// So : the code below was rewritten to remove all usage of NAString
// objects.  
//
//
// $$$ NB: ENSCRIBE ERROR CODES NOT CHECKED !!!
//
// Currently, the class NAClusterInfo calls a private method
// ::filloutDisks(), which calls a few subroutines from Enscribe.
// However, these calls ignore the return values from Enscribe, and this
// is clearly incorrect behavior.  Someday (soon ...?) we need to change
// the signature of the NAClusterInfo ctors so that the user of this class
// can say whether we should handle (i.e., longjmp on) fatal errors
// returned from Enscribe.  Given this parameter to the method
// ::filloutDisks(), we then will know whether we need to check these
// return error codes or not.

#include "NAClusterInfo.h"
#include "NADefaults.h"
#include "CompException.h"
#include <cextdecs/cextdecs.h>
#include <limits.h>

//str
#include <string.h>
//#include "NAStringDef.h"
//str

#include <stdio.h>
#include "guardian/pdctctlz.h"
#include "nsk/nskport.h"

#include "NATable.h"
#include "SchemaDB.h"

#include "ComRtUtils.h"


#include "OptimizerSimulator.h"
// Global pointer to OptimizerSimulator
#include "CmpErrors.h"

#include "seabed/ms.h"
#include <cstdlib>

ULng32 dp2DescHashFunc(const DP2name& name);

THREAD_P NABoolean gIsStaticCompiler = FALSE;
void SetStaticCompiler(NABoolean isStaticCompiler)
{
  gIsStaticCompiler = isStaticCompiler;
}

// LCOV_EXCL_START
NABoolean IsStaticCompiler()
{
  return gIsStaticCompiler;
} 
// LCOV_EXCL_STOP

NABoolean fileExists(const char *filename, NABoolean & isDir);

//------------------------------------------------------------------------
// Global pointer to cluster information is initially null and remains so
// until actually required.
//------------------------------------------------------------------------
THREAD_P NAClusterInfo* gpClusterInfo = NULL;
//<pb>
//==============================================================================
//  Set up global pointer to cluster information if this hasn't been done 
// previously.
//
// Input:
//  none
//
// Output:
//  none
//
// Return:
//  none
//
//==============================================================================
void
setUpClusterInfo(CollHeap* heap)
{
  #ifndef NDEBUG
  // LCOV_EXCL_START
  if (getenv("NO_SERVICES"))  // KSKSKS
    return;                   // KSKSKS
  // LCOV_EXCL_STOP
  #endif

  //-------------------------------------------
  // Return now if cluster info already set up.
  //-------------------------------------------
  if (gpClusterInfo)
    return;

  //-------------------------------------------------------
  // Set up cluster information based on hardware platform.
  //-------------------------------------------------------
  if (OSIM_runningSimulation())
  {
    switch (CURRCONTEXT_OPTSIMULATOR->getCaptureSysType())
    {
    case OptimizerSimulator::OSIM_LINUX:
      gpClusterInfo = new (heap) NAClusterInfoLinux (heap);
      break;
    case OptimizerSimulator::OSIM_WINNT:
      // Until OSIM captures are implemented on NT, the
      // creation of NAClusterInfoNT objects on other
      // platforms is not possible.
      CMPASSERT(0);
      break;
    case OptimizerSimulator::OSIM_NSK:
      gpClusterInfo = new (heap) NAClusterInfoNSK (heap);
      break;
    case OptimizerSimulator::OSIM_UNKNOWN_SYSTYPE:
    default:
      CMPASSERT(0); // Case not handled
      break;
    }
  }
  else
  {
    gpClusterInfo = new (heap) NAClusterInfoLinux (heap);
  }
}
//<pb>

//---------------------------------------------------------
//DP2name is a wrapper class for fully specified dp2 names. 
//We cannot hash on primitives like characters.
//--------------------------------------------------------
DP2name::DP2name(char* dp2name, CollHeap* heap)
: heap_(heap)
{
  dp2name_=new(heap_) (char[strlen(dp2name)+1]);
  strcpy(dp2name_,dp2name);
}

void DP2name::getDp2Name(char* &name) const
{
  name = new (CmpCommon::statementHeap()) char[strlen(dp2name_)+1];
  strcpy(name,dp2name_);
}

inline NABoolean DP2name::operator==(const DP2name &dp2Name)
{
  if(strcmp(dp2Name.dp2name_,dp2name_)==0) return TRUE;
  return FALSE;
}

// LCOV_EXCL_START
ULng32 DP2name::hash() const 
{
  return dp2DescHashFunc(*this);
}
// LCOV_EXCL_STOP

DP2name::~DP2name()
{
  NADELETEBASIC(dp2name_,heap_);
}

//------------------------------------------------------------
//DP2Info is a wrapper class for cluster and CPU information for a 
//particular DP2. As a member it has a DP2Name and 3 integers 
//representing the cluster, primary CPU and secondary CPU associated 
//with the DP2.
//-------------------------------------------------------------  

DP2info::DP2info(Lng32 clusterNum, Lng32 primary, Lng32 secondary)
{
  clusterNumber_=clusterNum;
  primaryCPU_=primary;
  secondaryCPU_=secondary;

}

void DP2info::getDp2Info(Int32  & clusterNum,
                         Int32  & primary,
                         Int32  & secondary)
{
  clusterNum=clusterNumber_;
  primary=primaryCPU_;
  secondary=secondaryCPU_;
}

//<pb>


//  hash function for dp2name
ULng32 dp2DescHashFunc(const DP2name& name) 
{
  char * dp2name;
 
  // method getDp2Name allocates an array of char for dp2name
  name.getDp2Name(dp2name);

  ULng32 index=0;
  size_t nameLen = strlen(dp2name);
  for(CollIndex i=0;i<=nameLen;i++){
    index += (unsigned char) dp2name[i];
  }

  NADELETEBASIC(dp2name, CmpCommon::statementHeap());

  return index;
}

//hash funtion for tableIdentifier passed in from NATable
ULng32 tableIdentHashFunc( const CollIndex& ident)
{
  return (ULng32)(ident);
}
//hash function for clusterNumber
ULng32 clusterNumHashFunc(const CollIndex& num)
{
  return (ULng32)(num);
}



//============================================================================
// Methods for class NAClusterInfo; it provides information about the cluster
// in which we are running.
//============================================================================

//============================================================================
//  NAClusterInfo constructor.
//
// Input: heap pointer(should always be context heap
//  
//
// Output: Retrieves information for the local cluster. This includes information
// regarding its CPUs as well as for the dp2s. All these information will be cached
// in the appropriate structure.
//
// Return:
//  none
//
//==============================================================================
NAClusterInfo::NAClusterInfo(CollHeap * heap)
 : heap_(heap), 
   maxOSV_(COM_VERS_UNKNOWN),
   maxOSVName_(heap),
   inTestMode_(FALSE),
   useAggregationNodesOnly_(FALSE)
{
  OptimizerSimulator::osimMode mode = OptimizerSimulator::OFF;

  if(!CURRCONTEXT_OPTSIMULATOR->isCallDisabled(9))
    mode = CURRCONTEXT_OPTSIMULATOR->getOsimMode();

  // Check for OSIM mode
  switch (mode)
  {
    case OptimizerSimulator::OFF:
    case OptimizerSimulator::CAPTURE:
    {
      dp2NameToInfoMap_ = new(heap) NAHashDictionary<DP2name,DP2info>
          (&dp2DescHashFunc, 101,TRUE,heap);
      clusterToCPUMap_ = new(heap) NAHashDictionary<CollIndex,maps>
          (&clusterNumHashFunc,17,TRUE, heap);
      tableToClusterMap_ = new(heap) NAHashDictionary<CollIndex, maps> 
          (&tableIdentHashFunc, 13, TRUE, heap);

      activeClusters_= NULL;
      smpCount_ = -1;

      NADefaults::getNodeAndClusterNumbers(localSMP_ , localCluster_);


#if 0 // pre SQF SQF_0_6_1_02Mar10, get info about physical nodes 
      MS_Mon_Node_Info_Type nodeInfo;
      memset(&nodeInfo, 0, sizeof(nodeInfo));

      Int32 error = msg_mon_get_node_info_all(&nodeInfo);
      CMPASSERT(error == 0);
#endif

      Int32 nodeCount = 0;
      Int32 nodeMax = 0;
      MS_Mon_Node_Info_Entry_Type *nodeInfo = NULL;

      // Get the number of nodes to know how much info space to allocate
      Int32 error = msg_mon_get_node_info(&nodeCount, 0, NULL);
      CMPASSERT(error == 0);
      CMPASSERT(nodeCount > 0);

      // Allocate the space for node info entries
      nodeInfo = (MS_Mon_Node_Info_Entry_Type *) new(heap) 
                  char[nodeCount * sizeof(MS_Mon_Node_Info_Entry_Type)];
      CMPASSERT(nodeInfo);


      // Get the node info
      memset(nodeInfo, 0, sizeof(nodeInfo));
      nodeMax = nodeCount;
      error = msg_mon_get_node_info(&nodeCount, nodeMax, nodeInfo);
      CMPASSERT(error == 0);

      maps *cpuList=new(heap) maps(heap);
      smpCount_ = 0;

      NAList<CollIndex> storageList(nodeCount);

      for (Int32 i = 0; i < nodeCount; i++)
      {
        if (nodeInfo[i].spare_node)
          continue;

        // The zone type must either be an aggregation node or storage node
        // to be included in the list of CPUs.
        if ((nodeInfo[i].type & MS_Mon_ZoneType_Aggregation) != 0 ||
            ((nodeInfo[i].type & MS_Mon_ZoneType_Storage) != 0 ))
        {
          if ( (nodeInfo[i].type & MS_Mon_ZoneType_Storage) != 0 )
            storageList.insert(nodeInfo[i].nid);

          if ( (nodeInfo[i].type & MS_Mon_ZoneType_Storage) == 0 )
            cpuList->insertToAggregationNodeList(nodeInfo[i].nid);

          if (!nodeInfo[i].spare_node)
             smpCount_++;
        }
      }

      // Fix Bugzilla #1210. Put the aggregation nodes at the beginning of 
      // the list. ESP logical node map synthesization code can take the 
      // advantage of this and place more ESPs on aggregation nodes when 
      // the node map size is less than the total number of SQL nodes.
      *(cpuList->list) = *(cpuList->listOfAggregationOnlyNodes);
      cpuList->list->insert(storageList);

      // if there exists no aggregation only nodes, allow all nodes to
      // host esps.
      if (cpuList->listOfAggregationOnlyNodes->entries() == 0) {
        for (Int32 i = 0; i<cpuList->list->entries(); i++)
          cpuList->insertToAggregationNodeList((*(cpuList->list))[i]);
      }

      NADELETEBASIC(nodeInfo, heap);


      CollIndex *ptrClusterNum = new(heap) CollIndex(localCluster_);
      CollIndex *cluster=clusterToCPUMap_->insert(ptrClusterNum,cpuList); 

      CMPASSERT(cluster);
      fillOutDisks_(heap_);
      // For CAPTURE mode, the data will be captured later in CmpMain::compile()
      break;
    }
    case OptimizerSimulator::SIMULATE:

      dp2NameToInfoMap_  = NULL;
      clusterToCPUMap_ = NULL;
      tableToClusterMap_ = NULL;
      activeClusters_= NULL;
      smpCount_ = -1;

      // Simulate the NAClusterInfo.
      simulateNAClusterInfo();
      break;
    default:
      // The OSIM must run under OFF (normal), CAPTURE or SIMULATE mode.
      OSIM_errorMessage("Invalid OSIM mode - It must be OFF or CAPTURE or SIMULATE.");
      break;
  }
} // NAClusterInfo::NAClusterInfo()

NAClusterInfo::~NAClusterInfo()
{
  // clear and delete dp2NameToInfoMap_
  if(dp2NameToInfoMap_)
  {
    dp2NameToInfoMap_->clearAndDestroy();
    delete dp2NameToInfoMap_;
  }
  CollIndex *key;  
  maps * value;
  UInt32 i=0;

  if(clusterToCPUMap_)
  {
    // clear and delete clusterToCPUMap_
    //iterate over all the entries in clusterToCPUMap_
    NAHashDictionaryIterator<CollIndex,maps> clusterToCPUMapIter(*clusterToCPUMap_);

    for ( i = 0 ; i < clusterToCPUMapIter.entries() ; i++)
    {
       clusterToCPUMapIter.getNext (key,value) ;
       NADELETEBASIC(key,CmpCommon::contextHeap());
       delete value;
    }
    clusterToCPUMap_->clear();
    delete clusterToCPUMap_;
  }

  if(tableToClusterMap_)
  {
    // clear and delete tableToClusterMap_
    //iterate over all the entries in tableToClusterMap_
    NAHashDictionaryIterator<CollIndex,maps> tableToClusterMapIter(*tableToClusterMap_);

    for ( i = 0 ; i < tableToClusterMapIter.entries() ; i++)
    {
       tableToClusterMapIter.getNext (key,value) ;
       NADELETEBASIC(key,CmpCommon::contextHeap());
       delete value;
    }
    tableToClusterMap_->clear();
    delete tableToClusterMap_;
  }

  // clear and delete activeClusters_ list
  if(activeClusters_)
  {
    activeClusters_->clear();
    delete activeClusters_;
  }

}

//============================================================================
// This method writes the information related to the NAClusterInfo class to a
// logfile called "NAClusterInfo.txt".
//============================================================================
void NAClusterInfo::captureNAClusterInfo()
{
  CollIndex i, ci;
  char filepath[OSIM_PATHMAX];
  char filename[OSIM_FNAMEMAX];

  // We don't capture data members that are computed during the compilation of
  // a query. These include:
  //
  // * smpCount_;
  // * tableToClusterMap_;
  // * activeClusters_;
  //

  // Construct an absolute pathname for output file.
  strcpy(filepath, CURRCONTEXT_OPTSIMULATOR->getOsimLogdir());
  strcpy(filename, "/NAClusterInfo.txt");
  strcat(filepath, filename);
  ofstream naclfile(filepath);

  naclfile << "localCluster_: " << localCluster_ << endl
           << "localSMP_: " << localSMP_ << endl;

  DP2name *key_dp2name;
  DP2info *val_dp2info;
  // Iterator for logging all the entries in dp2NameToInfoMap_ HashDictionary.
  NAHashDictionaryIterator<DP2name, DP2info> dp2N2IIter (*dp2NameToInfoMap_, NULL, NULL);
  char *dp2name;
  Int32 clusterNum, primaryCPU, secondaryCPU;

  naclfile << "dp2NameToInfoMap_: " << dp2N2IIter.entries() << " :" << endl;
  if (dp2N2IIter.entries() > 0)
  {
    // Write the header line for the table.
    naclfile << "  ";
    naclfile.width(17); naclfile << "dp2name_" << "  ";
    naclfile.width(14); naclfile << "clusterNumber_" << "  ";
    naclfile.width(11); naclfile << "primaryCPU_" << "  ";
    naclfile.width(13); naclfile << "secondaryCPU_" << endl;
    for (i=0; i<dp2N2IIter.entries(); i++)
    {
      dp2N2IIter.getNext(key_dp2name, val_dp2info);
      key_dp2name->getDp2Name(dp2name);
      val_dp2info->getDp2Info(clusterNum, primaryCPU, secondaryCPU);
      naclfile << "  ";
      naclfile.width(17); naclfile << dp2name << "  ";
      naclfile.width(14); naclfile << clusterNum << "  ";
      naclfile.width(11); naclfile << primaryCPU << "  ";
      naclfile.width(13); naclfile << secondaryCPU << endl;
    }
  }

  CollIndex *key_collindex;  
  maps *val_maps;
  // Iterator for logging all the entries in clusterToCPUMap_ HashDictionary.
  NAHashDictionaryIterator<CollIndex, maps> C2CPUIter (*clusterToCPUMap_, NULL, NULL);  
  naclfile << "clusterToCPUMap_: " << C2CPUIter.entries() << " :" << endl;
  if (C2CPUIter.entries() > 0)
  {
    // Write the header line for the table.
    naclfile << "  ";
    naclfile.width(10); naclfile << "clusterNum" << "  ";
                        naclfile << "cpuList" << endl;
    for (i=0; i<C2CPUIter.entries(); i++)
    {
      C2CPUIter.getNext(key_collindex, val_maps);
      naclfile << "  ";
      naclfile.width(10); naclfile << *key_collindex << "  ";
                          naclfile << val_maps->list->entries() << " : ";
      for (ci=0; ci<val_maps->list->entries(); ci++)
      {
        naclfile.width(3); naclfile << (*(val_maps->list))[ci] << " ";
      }
      naclfile << endl;
    }
  }
  naclfile << "discsOnCluster_: " << discsOnCluster_  << endl;
  naclfile.close();

  // Now save the OS-specific information to the NAClusterInfo.txt file
  captureOSInfo();
}

//============================================================================
// This method reads the information needed for NAClusterInfo class from
// a logfile called "NAClusterInfo.txt" and then populates the variables
// accordigly.
//============================================================================
void NAClusterInfo::simulateNAClusterInfo()
{
  Int32 i, ci;
  char var[256];
  char filepath[OSIM_PATHMAX];
  char filename[OSIM_FNAMEMAX];

  // initialize stuff that is not read from the log file
  // this info is calculated during query compilation  
  tableToClusterMap_ = new(heap_) NAHashDictionary<CollIndex, maps>
      (&tableIdentHashFunc, 13, TRUE, heap_);

  activeClusters_= NULL;
  smpCount_ = -1;

  // Construct an absolute pathname for input file.
  strcpy(filepath, CURRCONTEXT_OPTSIMULATOR->getOsimLogdir());
  strcpy(filename, "/NAClusterInfo.txt");
  strcat(filepath, filename);
  NABoolean isDir;

  if(!fileExists(filepath,isDir))
  {
    char errMsg[38+OSIM_PATHMAX+1]; // Error msg below + filename + '\0'
    // LCOV_EXCL_START
    snprintf(errMsg, sizeof(errMsg), "Unable to open %s file for reading data.", filepath);
    OsimLogException(errMsg, __FILE__, __LINE__).throwException();
    // LCOV_EXCL_STOP
  }

  ifstream naclfile(filepath);

  while(naclfile.good())
  {
    // Read the variable name from the file.
    naclfile.getline(var, sizeof(var), ':');
    if(!strcmp(var, "localCluster_"))
    {
      naclfile >> localCluster_; naclfile.ignore(OSIM_LINEMAX, '\n');
    }
    else if (!strcmp(var, "localSMP_"))
    {
      naclfile >> localSMP_; naclfile.ignore(OSIM_LINEMAX, '\n');
    }
    else if (!strcmp(var, "dp2NameToInfoMap_"))
    {
      char dp2name[100];
      Int32 dp2N2I_entries, clusterNumber, primaryCPU, secondaryCPU, cpuLimit;
      dp2NameToInfoMap_  = new(heap_)  NAHashDictionary<DP2name,DP2info> (&dp2DescHashFunc, 101,TRUE,heap_);
      naclfile >> dp2N2I_entries; naclfile.ignore(OSIM_LINEMAX, '\n');
      if (dp2N2I_entries > 0)
      {
        // Read and ignore the header line.
        naclfile.ignore(OSIM_LINEMAX, '\n');
        for (i=0; i<dp2N2I_entries; i++)
        {
          naclfile >> dp2name >> clusterNumber >> primaryCPU >> secondaryCPU;
          naclfile.ignore(OSIM_LINEMAX, '\n');
          // Validate legitimacy of SMP node number.

         
          if (OSIM_isLinuxbehavior())
             cpuLimit = MAX_NUM_SMPS_SQ;
          else
             cpuLimit = MAX_NUM_SMPS_NSK;
  
          CMPASSERT(primaryCPU >= 0 && primaryCPU <= cpuLimit);
          DP2name *key_dp2name = new(heap_) DP2name(dp2name, heap_);
          DP2info *val_dp2info = new(heap_) DP2info(clusterNumber, primaryCPU, secondaryCPU);
          DP2name *checkDp2name = dp2NameToInfoMap_->insert(key_dp2name, val_dp2info);
          CMPASSERT(checkDp2name);
        }
      }
    }
    else if (!strcmp(var, "clusterToCPUMap_"))
    {
      Int32 C2CPU_entries, clusterNum, cpuList_entries, cpuNum;

      clusterToCPUMap_ = new(heap_) NAHashDictionary<CollIndex,maps>(&clusterNumHashFunc,17,TRUE, heap_);
      naclfile >> C2CPU_entries; naclfile.ignore(OSIM_LINEMAX, '\n');
      if(C2CPU_entries > 0)
      {
        // Read and ignore the header line.
        naclfile.ignore(OSIM_LINEMAX, '\n');
        for (i=0; i<C2CPU_entries; i++)
        {
          naclfile >> clusterNum;
          naclfile >> cpuList_entries; naclfile.ignore(OSIM_LINEMAX, ':');
          CollIndex *key_clusterNum = new(heap_) CollIndex(clusterNum);
          maps *val_cpuList = new(heap_) maps(heap_);
          for (ci=0; ci<cpuList_entries; ci++)
          {
            naclfile >> cpuNum;
            val_cpuList->list->insert(cpuNum);
          }
          naclfile.ignore(OSIM_LINEMAX, '\n');
          CollIndex *checkClusterNum = clusterToCPUMap_->insert(key_clusterNum, val_cpuList);
          CMPASSERT(checkClusterNum);
        }
      }
    }
    else if (!strcmp(var, "discsOnCluster_"))
    {
      naclfile >> discsOnCluster_; naclfile.ignore(OSIM_LINEMAX, '\n');
    }
    else
    {
      // This variable will either be read in simulateNAClusterInfoNSK()
      // method of NAClusterInfoNSK class or is not the one that we want
      // to read here in this method. So discard it and continue.
      naclfile.ignore(OSIM_LINEMAX, '\n');
      while (naclfile.peek() == ' ')
      {
        // The main variables are listed at the beginning of a line
        // with additional information indented. If one or more spaces
        // are seen at the beginning of the line upon the entry to this
        // while loop, it is because of that additional information.
        // So, ignore this line since the variable is being ignored.
        naclfile.ignore(OSIM_LINEMAX, '\n');
      }
    }
  }
}

//<pb>
//==============================================================================
//  An internal function which finds out all DP2s in the local system and puts the
// information it finds in dp2NameToInfoMap
//
// Input:
//  heap - Pointer to heap in which to allocate memory.
//
// Output:
//  none
//
// Return:
//  none
//
// To Be Done: 
//  This method does not check the return calls from the filesystem to see
// if there have been any serious errors.  These errors should be checked.
// However, not all users of NAClusterInfo care about these errors, so we
// need to change the NAClusterInfo ctor so that the user communicates
// whether s/he wants to do a longjmp in case of a bad error from the
// filesystem subroutines.
//
//==============================================================================
#pragma nowarn(770)   // warning elimination 
#pragma nowarn(252)   // warning elimination 
#pragma nowarn(1506)   // warning elimination 
#pragma nowarn(259)   // warning elimination 
void
NAClusterInfo::fillOutDisks_(CollHeap * heap)
{

  Int32 dNumInfo = 0;
  MS_Mon_Process_Info_Type* tseInfo = NULL;

  short ret_val = msg_mon_get_process_info_type(MS_ProcessType_TSE,
                                             &dNumInfo,
                                             0,  // max ignored if info is NULL
                                             NULL); 

  tseInfo = new (heap_) MS_Mon_Process_Info_Type [dNumInfo];

  ret_val = msg_mon_get_process_info_type(MS_ProcessType_TSE,
                                             &dNumInfo,
                                             dNumInfo,
                                             tseInfo);

  discsOnCluster_= 0;

  // ret_val may need error handling.

  // When msg_mon_get_process_info_type() returns, each of the "process_name"
  // fields in the "tseInfo" array are filled in.  As each TSE is processed,
  // the rest of the array is searched to find the other TSE in the backup/
  // primary pair.  If the other TSE in the pair is found, the "proces_name"
  // in it is replaced with a null character to prevent it from being
  // processed twice.

  for (Int32 tseIdx1 = 0; tseIdx1 < dNumInfo; tseIdx1++)
  {
    // Skip this TSE if it has already been processed.
    if (tseInfo[tseIdx1].process_name[0] == '\0')
      continue;

    Lng32 primaryNode = -1;
    Lng32 backupNode = -1;

    if (tseInfo[tseIdx1].backup)
      backupNode = tseInfo[tseIdx1].nid;
    else
      primaryNode = tseInfo[tseIdx1].nid;

    // Examine the rest of the array for a TSE with a matching volume name.
    for (Int32 tseIdx2 = tseIdx1+1; tseIdx2 < dNumInfo; tseIdx2++)
    {
      if (strcmp(tseInfo[tseIdx1].process_name, tseInfo[tseIdx2].process_name) == 0)
      {
        // A match was found. Set the first character of the process_name to
        // a null character so it won't be processed again.
        tseInfo[tseIdx2].process_name[0] = '\0';

        if (tseInfo[tseIdx2].backup)
        {
          CMPASSERT(backupNode == -1);  // There shouldn't be two backup TSEs
          backupNode = tseInfo[tseIdx2].nid;
        }
        else
        {
          CMPASSERT(primaryNode == -1); // There shouldn't be two primary TSEs
          primaryNode = tseInfo[tseIdx2].nid;
        }
        break; // break from tseIdx2 loop
      }
    }


    char fullName[100];
    snprintf(fullName, sizeof(fullName), "\\NSK.%s", tseInfo[tseIdx1].process_name);
    DP2name * ptrDp2name = new(heap) DP2name(fullName, heap);
    DP2info * dp2Info = new(heap) DP2info(localCluster_,
                                          primaryNode,
                                          backupNode);
    DP2name *check = dp2NameToInfoMap_->insert(ptrDp2name,dp2Info);
    CMPASSERT(check);  // The assertion will fail on duplicate dp2 names
    discsOnCluster_++;
  }

  NADELETEBASIC(tseInfo, heap_);


} // NAClusterInfo::fillOutDisks_()
#pragma warn(259)  // warning elimination 
#pragma warn(1506)  // warning elimination
#pragma warn(252)  // warning elimination
#pragma warn(770)  // warning elimination


#pragma nowarn(1506)   // warning elimination

// LCOV_EXCL_START
Lng32
NAClusterInfo::getNumActiveCluster()
{
  if(NOT activeClusters_) createActiveClusterList();
  CMPASSERT(activeClusters_->entries());
  return activeClusters_->entries();
}// NAClusterInfo::getNumActiveClusters()
// LCOV_EXCL_STOP

#pragma warn(1506)  // warning elimination 

/*------------------------------------------------------------- 
NAClusterInfo::createActiveClusterList()
 Helper function for getSuperNodeMap() 
 Goes through the following algorithm to identify 
the all the active clusters for the current statement if
value for REMOTE_ESP_PARALLELISM is SYSTEM.
It also identifies active clusters for 'ON' or 'OFF'.

ALGORITHM implemented by SQL/MX:

Query involves table A, B, C and D. 

Base table for A is distributed on systems 1, 2, 5
Base table for B is distributed on systems 3, 6
Base table for C is distributed on systems 2, 3
Base table for D is distributed on systems 4

Target systems 1,2 3, 4, 5, 6

SQL/MX will first go through the list of tables and find 
the tables that have a system common with any other table in 
the list. In this case table A has system 2 common with table C 
and vice versa and table B has system 3 common with table C and
 vice versa. Now we take the super set of systems for Table A, B 
 and C which gives us ( 1, 2, 3, 5, 6) as active systems. 
 Target system 4 got excluded.

Another example:
	A typical star join schema where the dimension tables 
        are small and the fact table is large. 

Fact table is distributed on 1, 2, 3
Dimension one table is on 1
Dimension two table is on 2

Scope chosen by SQL/MX will be 1, 2, 3 

SQL/MX is going to bring up ESPs on CPUs in all these systems and 
generate as much parallelism it can. At the same time, it tries 
to make sure that scan ESPs are co-located to be nearest to their 
dp2s or active partitions, reducing remote communication.

Special Cases:
	
a) If it is a single table query then all the target systems will be considered active.
 
b)	If the algorithm cannot come up with any active system and 
the local system is not completely restricted by the user then ESPs 
will be brought up only on the local system. On the other hand, if 
the local system is unavailable ( CPU map completely restricts the 
use of local system) then no ESP will be brought up and as a result 
there will be no parallel processing. 

------------------------------------------------------------------*/
#pragma nowarn(1506)   // warning elimination 
#pragma nowarn(262)   // warning elimination 
void 
NAClusterInfo::createActiveClusterList()
{
  CMPASSERT(tableToClusterMap_);
  activeClusters_ = new(CmpCommon::statementHeap()) 
    NAList<CollIndex>(CmpCommon::statementHeap());

  if(OSIM_isNSKbehavior())
  {
  NADefaults & defs = ActiveSchemaDB()->getDefaults();
  NAString allocationChoice;
  defs.getValue(REMOTE_ESP_ALLOCATION,allocationChoice);
  NAString::caseCompare cmp = NAString::ignoreCase;
  
  // Look up clusters with table identifiers in tableToClusterMap_

  const NATableDB *tableDB = ActiveSchemaDB()->getNATableDB();
  // get a list of table identifiers allocated on statement heap.
  const LIST(CollIndex) & tableIdList = 
    tableDB->getStmtTableIdList(CmpCommon::statementHeap());

    CollIndex i = 0;
// LCOV_EXCL_START
  if(allocationChoice.compareTo("ON")==0)
  {
    //selects all the target clusters as active. 
    //checks for duplication
      for(i = 0; i < tableIdList.entries(); i++)
    {
      CollIndex tableIdent = tableIdList[i];
      maps *clusterList = tableToClusterMap_->getFirstValue(&tableIdent);
      CMPASSERT(clusterList);
      for(CollIndex j = 0; j < clusterList->list->entries(); j++)
      {
	if(  NOT activeClusters_->contains( (*(clusterList->list))[j] )  )
	{
	  activeClusters_->insert( (*(clusterList->list))[j] );
	}
      }

    }
  }
// LCOV_EXCL_STOP
  else if(allocationChoice.compareTo("SYSTEM")==0)
  {
    //Only one table in the query every target cluster is active
    if(tableIdList.entries() == 1) 
    {
      CollIndex tableIdent = tableIdList[0];
      maps *clusterList = tableToClusterMap_->getFirstValue(&tableIdent);
      activeClusters_->insert(*(clusterList->list));
    }
    else
    {
      //Psuedo code 
      //iterate through every table's clusterlist
      //{ 
      //  insert the cluster into a list and maintain a corresponding 
      //  counter for the list .
      // ex. list 1  list2 
      //      
      //      156     1
      //      188     2
      //      192     1
      //   This means cluster 188 appeared twice among the tables whereas other
      //  appeared only once
      //}
      // iterate through every table's clusterlist
      // {
      //   if a cluster from a table has counter of 2 or more then we know that 
      //   the cluster is common among two tables. In this case add all the clusters 
      //   from the table under consideration. and so on. 
      // }
      //
      
      
      NAList<CollIndex> * targetClusters = new (CmpCommon::statementHeap())
                                NAList<CollIndex> (CmpCommon::statementHeap());

      NAList<CollIndex> * clusterCount = new (CmpCommon::statementHeap())
                                NAList<CollIndex> (CmpCommon::statementHeap());

        for(i = 0; i < tableIdList.entries(); i++)
      {
	CollIndex tableIdent = tableIdList[i];
	maps *clusterList = tableToClusterMap_->getFirstValue(&tableIdent);
	CMPASSERT(clusterList);
	for(CollIndex j = 0; j < clusterList->list->entries(); j++)
        {
          Int32 index = targetClusters->index((*(clusterList->list))[j]);
          if(index != NULL_COLL_INDEX)
          {
            (*clusterCount)[index]++;
          }
          else
          {
            targetClusters->insertAt(targetClusters->entries(),
				     (*(clusterList->list))[j]);
            clusterCount->insertAt(clusterCount->entries(),1);
          }
        }
      }
      
      for(CollIndex i = 0; i < tableIdList.entries(); i++)
      {
	CollIndex tableIdent = tableIdList[i];
	maps *clusterList = tableToClusterMap_->getFirstValue(&tableIdent);
	CMPASSERT(clusterList);
	NABoolean includeClusterListForThisTable = FALSE;
	for(CollIndex j = 0; j < clusterList->list->entries(); j++)
        {
	  Int32 index = targetClusters->index( (*(clusterList->list))[j] );
	  CMPASSERT(index != NULL_COLL_INDEX);
          if( (*clusterCount)[index] >1 )
	  {
	    includeClusterListForThisTable = TRUE;
            break;
	  }
        }
        
	if(includeClusterListForThisTable) 
	{
	  for(CollIndex j = 0; j < clusterList->list->entries(); j++)
	    {
	      if( NOT activeClusters_->contains( (*(clusterList->list))[j] ) )
	      {
		activeClusters_->insert( (*(clusterList->list))[j] );
	      }
	    }
	}
      } // for
      
    } // else
  } // else if

// LCOV_EXCL_START
  //No active cluster was found so local cluster is active
  if( NOT activeClusters_->entries() ) 
  {
    // This needs to be done because for some statements we will
    // come over here although there is no tables in the query
    // It also covers the REMOTE_ESP_PARALLELISM is 'OFF' case
    activeClusters_->insert(localCluster_);
  }
// LCOV_EXCL_STOP
  }
  else{
    // Linux and NT behavior
    activeClusters_->insert(localCluster_);
  }
  
}
#pragma warn(262)  // warning elimination
#pragma warn(1506)  // warning elimination 




//<pb>
//==============================================================================
//  Determine how many SMPs are available in the cluster.
//
// Input:
//  none
//
// Output:
//  none
//
// Return:
//  number of available SMPs in the cluster.
//
//==============================================================================
#pragma nowarn(1506)   // warning elimination
Int32
NAClusterInfo::computeNumOfSMPs()
{
    Int32 count =0;
    if(NOT activeClusters_) createActiveClusterList();

    for(CollIndex index=0; index < activeClusters_->entries(); index++)
    {
      maps *cpuList = clusterToCPUMap_->getFirstValue(&((*activeClusters_)[index]));
      if(cpuList) 
      {
        count += cpuList->getCpuCount(getUseAggregationNodesOnly());
        continue;
      }
    
#pragma warning (disable : 4244)   //warning elimination
      getProcessorStatus(cpuList,(*activeClusters_)[index]);
#pragma warning (default : 4244)   //warning elimination
      count +=cpuList->list->entries();
    }
   return count;
}

void NAClusterInfo::setUseAggregationNodesOnly(NABoolean x) 
{  
    if ( useAggregationNodesOnly_ != x )  {
       useAggregationNodesOnly_ = x; 
       computeNumOfSMPs();
    }
}

Int32
NAClusterInfo::numOfSMPs()
{
  if(smpCount_ <0)
    smpCount_ = computeNumOfSMPs();

  // This is temporary patch for PARALLEL_NUM_ESPS issue. This CQD should
  // be used in many places for costing, NodeMap allocation, synthesizing
  // physProperties and so on. But currently it is used only in
  // RelRoot::createContextForAChild() creating lots of discrepansies in 
  // the code. Sept. 2006

  // Get the value as a token code, no errmsg if not a keyword.
  if ( (CmpCommon::getDefault(COMP_BOOL_136) == DF_ON) AND
       (CmpCommon::getDefault(PARALLEL_NUM_ESPS, 0) != DF_SYSTEM)
     )
  {
    // -------------------------------------------------------------------
    // A value for PARALLEL_NUM_ESPS exists.  Use it for the count of cpus
    //  but don't exceed the number of cpus available in the cluster.
    // -------------------------------------------------------------------
    smpCount_ = MINOF(smpCount_, 
        (Int32)(ActiveSchemaDB()->getDefaults().getAsLong(PARALLEL_NUM_ESPS)));
  }

  return smpCount_; 

} // NAClusterInfo::numOfSMPs()  
#pragma warn(1506)  // warning elimination 

//----------------------------------------------------------
// getProcessorStatus()
// Input: clusterNumber
// Output: CPU's in the cluster that are running
//----------------------------------------------------------

void 
NAClusterInfo::getProcessorStatus(maps* &outcpuList,short clusterNum)
{
  if( OSIM_isNSKbehavior() )
  {
    maps * cpuList = new(heap_) maps(heap_);
    CollIndex *ptrClusterNum = new(heap_) CollIndex(clusterNum);

    Lng32 status = OSIM_REMOTEPROCESSORSTATUS(clusterNum);

    // We fill out smpStatus, which tells us which SMPs are alive at
    // compilation time.
    //--------------------------------------------------------------
    Lng32 numSMPs = MAX_NUM_SMPS_NSK;

    Lng32 numActiveSMPs = 0;
    
    Int32 i = 0;
    for (i = 0; i < numSMPs; i++)
    {
      if(status & (0x8000 >> i))
        numActiveSMPs = i + 1;
    }

    numSMPs = 1;
    while(numActiveSMPs > numSMPs)
      numSMPs = numSMPs << 1;

    for (i = 0; i < numSMPs; i++)
    {
      //if(status & (0x8000 >> i))
      //  cpuList->list->insert(CollIndex(i)); 
      //
      // Create plans that assume all CPUs are available, 
      // even when one is down.
      //
      cpuList->list->insert(CollIndex(i));
    }
  
    CollIndex *cluster=clusterToCPUMap_->insert(ptrClusterNum,cpuList);
    outcpuList = new(HEAP) maps(HEAP);
    *(outcpuList->list)  = *(cpuList->list);
  }
  else {
    CMPASSERT(0);
  }
}


//-----------------------------------------------------------------
//NAClusterInfo::getSuperNodeMap()
// called by NodeMap.cpp
// Returns the active clusters and their corresponding active CPUs
// clusterList and cpuList has one to one relationship i.e. cpuList[0]
// contains cpus for cluster in clusterList[0]
//-----------------------------------------------------------------
#pragma nowarn(1506)   // warning elimination 
NABoolean
NAClusterInfo::getSuperNodemap(NAArray<CollIndex>* &clusterList, 
                               NAArray<NAList<CollIndex>*>* &cpuList,
                               Int32 &cpuCount)
{
  cpuCount = 0;
  if(NOT activeClusters_) createActiveClusterList();
  clusterList = new(HEAP) NAArray<CollIndex>(HEAP,activeClusters_->entries());
  for(CollIndex entry =0; entry< activeClusters_->entries();entry++)
  {
    clusterList->insertAt(entry,(*activeClusters_)[entry]);
  }

#ifndef NDEBUG
// LCOV_EXCL_START
  if(getenv("debug_MNO"))
  {
    FILE * ofd = fopen("superNodeMap","ac");
    BUMP_INDENT(DEFAULT_INDENT);
    fprintf(ofd,"%s %s\n",NEW_INDENT,"Active Clusters: ");
    for(CollIndex n=0;n<activeClusters_->entries();n++)
    {
      fprintf(ofd, "%s %d \n", NEW_INDENT, (*activeClusters_)[n]); 
    }
    fprintf(ofd,"*********************************************************************\n");
    fprintf(ofd,"%s %s\n",    NEW_INDENT,"Active Cluster and its CPUs");
    fclose(ofd);
  }
// LCOV_EXCL_STOP
#endif

  cpuList= new(HEAP) NAArray<NAList<CollIndex>*> (HEAP,activeClusters_->entries());
  maps * cpuForCluster=NULL;

  for(CollIndex index = 0;index<activeClusters_->entries();index++)
  {
    cpuForCluster = (maps*)(clusterToCPUMap_->getFirstValue(&(*activeClusters_)[index]));
#pragma warning (disable : 4244)   //warning elimination
    if (NOT cpuForCluster)
    {
      getProcessorStatus(cpuForCluster,(*activeClusters_)[index]);
    }
#pragma warning (default : 4244)   //warning elimination
  
    NABoolean aggreNodeOnly = 
        CmpCommon::getDefault(ESP_ON_AGGREGATION_NODES_ONLY) == DF_ON;

    NAList<CollIndex>*  cList = cpuForCluster->getCpuList(aggreNodeOnly); 

    NAList<CollIndex> * ptrCpuForCluster = new(HEAP) NAList<CollIndex>(*cList,HEAP);
    cpuCount += cList->entries();

#ifndef NDEBUG
// LCOV_EXCL_START
  if(getenv("debug_MNO"))
  {
    FILE * ofd = fopen("superNodeMap","ac");
    BUMP_INDENT(DEFAULT_INDENT);
    fprintf(ofd,"%s %s %2d\n",NEW_INDENT,
                           "Active cluster  ",(*activeClusters_)[index]);
    fprintf(ofd,"%s %s", NEW_INDENT, "CPUS:  ");
    for(CollIndex m=0;m<ptrCpuForCluster->entries();m++)
    {
      fprintf(ofd,"%d %s",(*ptrCpuForCluster)[m],"  ");
    }
    fprintf(ofd,"\n");
    fclose(ofd);
  }
// LCOV_EXCL_STOP
#endif

    cpuList->insertAt(index,ptrCpuForCluster);
  }

#ifndef NDEBUG
// LCOV_EXCL_START
if(getenv("debug_MNO"))
{
  FILE * ofd = fopen("superNodeMap","ac");
  fprintf(ofd,"*********************************************************************\n");
  fclose(ofd);
}
// LCOV_EXCL_STOP
#endif

  return TRUE;

}

// Returns total number of CPUs (including down CPUs)
Lng32 NAClusterInfo::getTotalNumberOfCPUs()
{
  Lng32 cpuCount = 0;
  if (NOT activeClusters_) createActiveClusterList();

  for(CollIndex index = 0;index<activeClusters_->entries();index++)
  {
    maps *cpuForCluster = (maps*)
      (clusterToCPUMap_->getFirstValue(&(*activeClusters_)[index]));
    if (cpuForCluster && cpuForCluster->list)
      cpuCount += cpuForCluster->list->entries();
  }

#ifndef NDEBUG
// LCOV_EXCL_START
  if ( inTestMode() ) {
    NADefaults & defs = ActiveSchemaDB()->getDefaults();
    cpuCount = (Int32)(defs.getAsLong(POS_TEST_NUM_NODES));
  }
// LCOV_EXCL_STOP
#endif

  return cpuCount;
}

//-----------------------------------------------------------------
//NAClusterInfo::getTableNodeList()
// Called by NodeMap::getTableNodeList().
// Input is a table identifier.
// Output is the list of nodes.
//
const NAList<CollIndex>* NAClusterInfo::getTableNodeList(Int32 tableIdent) const
{
  CMPASSERT(tableToClusterMap_);
  // Should this be an CMPASSERT(tableIdent > 0) ?
  if ((tableIdent >0) && (tableToClusterMap_->entries() > 0))
  {
    CollIndex longTableId(tableIdent);
    maps * tableNodeList = tableToClusterMap_->getFirstValue(&longTableId);
    return (tableNodeList ? tableNodeList->list : NULL);
  }
  return NULL;
}

#pragma warn(1506)  // warning elimination 
//-------------------------------------------------------------------
//inserts table to cluster mapping into tableToClusterMap.
//Checks for duplicates. tableIdent assigned by NATable.cpp
//------------------------------------------------------------------
NABoolean 
NAClusterInfo::insertIntoTableToClusterMap(Int32 tableIdent, Int32 cluster)
{
  CMPASSERT(tableToClusterMap_);
  CollIndex longTableId(tableIdent);
  maps * clustList = tableToClusterMap_->getFirstValue(&longTableId);
  if( NOT clustList)
  {
    clustList = new(heap_) maps(heap_);
    clustList->list->insert(cluster);
    // make a copy of tableIdent

    CollIndex * ptrTableNum = new(heap_) CollIndex(tableIdent);
    if (tableToClusterMap_->insert(ptrTableNum,clustList))
    {
      return TRUE;
    }
    else
    {
      // The clustList and ptrTableNum were not inserted correctly.
      // Free them now to prevent a memory leak.
      // LCOV_EXCL_START
      NADELETEBASIC(ptrTableNum, heap_);
      delete clustList;
      return FALSE;
      // LCOV_EXCL_STOP
    }
  }
  else if (NOT clustList->list->contains(cluster))
  {
    clustList->list->insert(cluster);
  }
  return TRUE;
}

//<pb>
//==============================================================================
//  Determine SMP number of a specified dp2 or -1 if no such dp2 exists.
//
// Input:
//  dp2Name  --  name of specified disk process.
//
// Output:
//  cluster no,.primary and secondary CPU,
// caches this info in the appropriate structure
//
// Return:
//  
//
//==============================================================================
#pragma nowarn(252)   // warning elimination 
#pragma nowarn(262)   // warning elimination 
#pragma nowarn(1506)   // warning elimination 
NABoolean
NAClusterInfo::whichSMPANDCLUSTER(const char * dp2Name,Int32& cluster, Int32& primary,
                                  Int32& secondary, Int32 tableIdent)
{
  char * name = (char *) dp2Name;
  DP2name temp(name,HEAP);

  //check if already cached
  DP2info * info = dp2NameToInfoMap_->getFirstValue(&temp);
  if (info) {
    info->getDp2Info(cluster,primary,secondary);
    return insertIntoTableToClusterMap(tableIdent,cluster);
  }

  DP2name *ptrDp2Name = new(CmpCommon::contextHeap()) DP2name(name,CmpCommon::contextHeap());
  DP2info *dp2info = NULL;
  DP2name *check = NULL;

  if( !OSIM_isNSKbehavior() )
  {
    // The behavior of NT and Linux could execute this code
    cluster = 0;
    primary=-1;
    secondary=-1;
    dp2info = new(heap_) DP2info(cluster,primary,-1);
    check =dp2NameToInfoMap_->insert(ptrDp2Name,dp2info);

    if(!check) return FALSE;
  }
  else
  {
#if (defined (NA_LINUX) && defined (SQ_NEW_PHANDLE))
    short processhandle[32];
#else
    short processhandle[10];
#endif // NA_LINUX
    short pin,cpu;
    short error;

    error = OSIM_FILENAME_TO_PROCESSHANDLE_((char *)dp2Name,
                        (short)strlen(dp2Name),processhandle);

    if(!error) {
      error = OSIM_PROCESSHANDLE_DECOMPOSE_(processhandle, &cpu, &pin, &cluster);
    }

    // LCOV_EXCL_START
    if(IsRemoteNodeDown(error)){
      // Use the local cluster and cpu and generate warning
      cluster = localCluster_;
      cpu = 0;
      char buffer[256];
      const char *nodeName = GetNodeName(dp2Name, buffer, sizeof(buffer));

      // Dont issue the warning for phantom objects that were created by OSIM.
      if ((CmpCommon::getDefault(CREATE_OBJECTS_IN_METADATA_ONLY) == DF_OFF) &&
           !OSIM_runningSimulation())
      *CmpCommon::diags() << DgSqlCode(arkcmpRemoteNodeDownWarning)
	<< DgString0(nodeName) << DgInt0(error);
    }
    else if(error) {
      *CmpCommon::diags() << DgSqlCode(arkcmpFileSystemError) 
			  << DgInt0(error);
      return FALSE;
    }
    // LCOV_EXCL_STOP

    primary = cpu; 
    dp2info = new(heap_) DP2info(cluster,primary,-1);
    check =dp2NameToInfoMap_->insert(ptrDp2Name,dp2info);

    if(!check) return FALSE;
  }

  return insertIntoTableToClusterMap(tableIdent,cluster);
} // NAClusterInfo::whichSMP()

#pragma warn(1506)  // warning elimination 
#pragma warn(262)  // warning elimination 
#pragma warn(252)  // warning elimination 
//<pb>

// This method is used by CURRSTMT_OPTDEFAULTS->newMemoryLimit() only. The latter
// is not called in normal mode.
// LCOV_EXCL_START
Int32 
NAClusterInfo::discsOnCluster() const
{ 
#ifndef NDEBUG
    if ( inTestMode() ) {
      NADefaults & defs = ActiveSchemaDB()->getDefaults();
      return (Int32)(defs.getAsLong(POS_TEST_NUM_NODES)) *
             (Int32)(defs.getAsLong(POS_TEST_NUM_VOLUMES_PER_NODE));
    }
#endif
   return discsOnCluster_; 
}
// LCOV_EXCL_STOP

// return TRUE if incompatible version (MAX(OSV) > MIN(MXV))
#pragma nowarn(1506)   // warning elimination 

// LCOV_EXCL_START
NABoolean NAClusterInfo::checkIfMixedVersion()
{
  return FALSE;
}
// LCOV_EXCL_STOP
#pragma warn(1506)  // warning elimination 

// setMaxOSV should be called for all NATable in the current Statement
// before the versioning check.
void NAClusterInfo::setMaxOSV(QualifiedName &qualName, COM_VERSION osv)
{
  if((maxOSV_ < osv) OR
    (maxOSV_ == COM_VERS_UNKNOWN))
  {
    maxOSV_ = osv;
    maxOSVName_ = qualName;
  }
}

// LCOV_EXCL_START
NABoolean NAClusterInfo::checkIfDownRevCompilerNeeded()
{
  return FALSE;
}
// LCOV_EXCL_STOP

#pragma nowarn(161)   // warning elimination 
void NAClusterInfo::cleanupPerStatement()
{
  //After every statement activeClusters_ should be NULL 
  // because statement heap has been cleared already. 
  activeClusters_ = NULL;
  smpCount_ = -1;
  // reset the mebers for versioning support
  maxOSV_ = COM_VERS_UNKNOWN;
}
#pragma warn(161)  // warning elimination 

void NAClusterInfo::removeFromTableToClusterMap(CollIndex tableId)
{
  maps *m=NULL;
#pragma nowarn(769)
  CollIndex *cix=NULL;
  NAHashDictionaryIterator<CollIndex, maps> Iter(*tableToClusterMap_, &tableId,
                                                       NULL);
  tableToClusterMap_->remove(&tableId);
  Iter.getNext(cix,m);

#pragma warn(769)
  if (m != NULL)
    delete m;
  if (cix != NULL)
    NADELETEBASIC(cix,heap_);
}

void NAClusterInfo::initializeForOSIMCapture()
{
  char localNodeName[9];
  short actualNodeNameLen = 0;
  Int32 guardianRC;

  guardianRC = OSIM_NODENUMBER_TO_NODENAME_(-1,
                                            localNodeName,
                                            9-1, // leave room for NUL
                                            &actualNodeNameLen);
  localNodeName[actualNodeNameLen]='\0';

  UInt32 i=0;

  // clear out clusterToCPUMap_;
  if (clusterToCPUMap_)
  {
    CollIndex * clusterNum;
    maps * cpuMap;
    NAHashDictionaryIterator<CollIndex,maps> clusterToCPUMapIter
                                             (*clusterToCPUMap_);
    for (i=0; i<clusterToCPUMapIter.entries(); i++)
    {
      clusterToCPUMapIter.getNext(clusterNum,cpuMap);

      // only delete entries from other clusters
      if(*clusterNum != (CollIndex)localCluster_)
      {
        // On Linux, there is only one cluster. The following code will not be exercised. 
        // LCOV_EXCL_START
        clusterToCPUMap_->remove(clusterNum);
        NADELETEBASIC(clusterNum,heap_);
        delete cpuMap;
        // LCOV_EXCL_STOP
      }
    }

  }

  //clear out dp2NameToInfoMap_
  if ( dp2NameToInfoMap_ )
  {
    DP2name * dp2Name = NULL;
    DP2info * dp2Info = NULL;
    NAHashDictionaryIterator<DP2name,DP2info> dp2NameToInfoMapIter
                                              (*dp2NameToInfoMap_);
    for (i=0; i< dp2NameToInfoMapIter.entries(); i++)
    {
      dp2NameToInfoMapIter.getNext(dp2Name,dp2Info);

      char * dp2NameStr = NULL;
      dp2Name->getDp2Name(dp2NameStr);
      char * nodeNameEnd = strchr (dp2NameStr,'.');
      (*nodeNameEnd) = '\0';
      if(strcmp(localNodeName,dp2NameStr)!=0)
      {
        // On Linux, there is only one cluster. The following code will not be exercised. 
        // LCOV_EXCL_START
        dp2NameToInfoMap_->remove(dp2Name);
        delete dp2Name;
        delete dp2Info;
        // LCOV_EXCL_STOP
      }
    }
  }
}

//<pb>
//******************************************************************************
// Platform specific cluster information.
//******************************************************************************

NAClusterInfoNSK::NAClusterInfoNSK(CollHeap * heap) : NAClusterInfo(heap),
  pageSize_(4096UL)
{
  OptimizerSimulator::osimMode mode = OptimizerSimulator::OFF;

  if(!CURRCONTEXT_OPTSIMULATOR->isCallDisabled(9))
    mode = CURRCONTEXT_OPTSIMULATOR->getOsimMode();

  // Check for OSIM mode
  switch (mode)
  {
    // LCOV_EXCL_START
    case OptimizerSimulator::OFF:
    case OptimizerSimulator::CAPTURE:
      frequency_            = findFrequency();
      iorate_               = findIORate();
      pageSize_             = findPageSize();
      totalMemoryAvailable_ = findTotalMemoryAvailable();
      // For CAPTURE mode, the data will be captured later in CmpMain::compile()
      break;
    // LCOV_EXCL_STOP
    case OptimizerSimulator::SIMULATE:
      // Simulate the NAClusterInfo.
      simulateNAClusterInfoNSK();
      break;
    // LCOV_EXCL_START
    default:
      // The OSIM must run under OFF (normal), CAPTURE or SIMULATE mode.
      OSIM_errorMessage("Invalid OSIM mode - It must be OFF or CAPTURE or SIMULATE.");
      break;
    // LCOV_EXCL_STOP
  }
}

// LCOV_EXCL_START
NAClusterInfoNSK::~NAClusterInfoNSK()
{
}
// LCOV_EXCL_STOP
//============================================================================
// This method writes the information related to the NAClusterInfoNSK class
// to a logfile called "NAClusterInfo.txt".
//============================================================================

// LCOV_EXCL_START
void NAClusterInfoNSK::captureOSInfo() const
{
  char filepath[OSIM_PATHMAX];
  char filename[OSIM_FNAMEMAX];

  strcpy(filepath, CURRCONTEXT_OPTSIMULATOR->getOsimLogdir());
  strcpy(filename, "/NAClusterInfo.txt");
  strcat(filepath, filename);
  ofstream naclnskfile(filepath, ios::app);

  naclnskfile << "frequency_: " << frequency_ << endl
              << "iorate_: " << iorate_ << endl
              << "seekTime_: "<< seekTime_ << endl
              << "pageSize_: " << pageSize_ << endl
              << "totalMemoryAvailable_: " << totalMemoryAvailable_ << endl;
  naclnskfile.close();
}
// LCOV_EXCL_STOP


//============================================================================
// This method reads the information specific to NAClusterInfoNSK class from
// a logfile called "NAClusterInfo.txt" and then populates the variables
// accordigly.
//============================================================================
void NAClusterInfoNSK::simulateNAClusterInfoNSK()
{
  char var[256];
  char filepath[OSIM_PATHMAX];
  char filename[OSIM_FNAMEMAX];

  strcpy(filepath, CURRCONTEXT_OPTSIMULATOR->getOsimLogdir());
  strcpy(filename, "/NAClusterInfo.txt");
  strcat(filepath, filename);
  NABoolean isDir;

  if(!fileExists(filepath,isDir))
  {
    char errMsg[38+OSIM_PATHMAX+1]; // Error msg below + filename + '\0'
// LCOV_EXCL_START
    snprintf(errMsg,sizeof(errMsg), "Unable to open %s file for reading data.", filepath);
    OsimLogException(errMsg, __FILE__, __LINE__).throwException();
// LCOV_EXCL_STOP
  }

  ifstream naclnskfile(filepath);

  while(naclnskfile.good())
  {
    // Read the variable name from the file
    naclnskfile.getline(var, sizeof(var), ':');
    if(!strcmp(var, "frequency_"))
    {
      naclnskfile >> frequency_; naclnskfile.ignore(OSIM_LINEMAX, '\n');
    }
    else if (!strcmp(var, "iorate_"))
    {
      naclnskfile >> iorate_; naclnskfile.ignore(OSIM_LINEMAX, '\n');
    }
    else if (!strcmp(var, "seekTime_"))
    {
      naclnskfile >> seekTime_; naclnskfile.ignore(OSIM_LINEMAX, '\n');
    }
    else if (!strcmp(var, "pageSize_"))
    {
      naclnskfile >> pageSize_; naclnskfile.ignore(OSIM_LINEMAX, '\n');
    }
    else if (!strcmp(var, "totalMemoryAvailable_"))
    {
      naclnskfile >> totalMemoryAvailable_; naclnskfile.ignore(OSIM_LINEMAX, '\n');
    }
    else
    {
      // This variable either may have been read in simulateNAClusterInfo()
      // method of NAClusterInfo class or is not the one that we want to
      // read here in this method. So discard it.
      naclnskfile.ignore(OSIM_LINEMAX, '\n');
      while (naclnskfile.peek() == ' ')
      {
        // The main variables are listed at the beginning of a line
        // with additional information indented. If one or more spaces
        // are seen at the beginning of the line upon the entry to this
        // while loop, it is because of that additional information.
        // So, ignore this line since the variable is being ignored.
        naclnskfile.ignore(OSIM_LINEMAX, '\n');
      }
    }
  }
}

// LCOV_EXCL_START
Int32 NAClusterInfoNSK::findFrequency()
{
  // should not come here on other platforms
  CMPASSERT(FALSE);
  return 0;

}

float NAClusterInfoNSK::findIORate()
{
  // should not come here on other platforms
  CMPASSERT(FALSE);
  return 0;
}
// LCOV_EXCL_STOP


//---------------------------------------------------------------------
// Returns the procesor frequency, in Megahertz, of this SMP
//---------------------------------------------------------------------
Int32 NAClusterInfoNSK::processorFrequency() const
{
  return frequency_;
}

//---------------------------------------------------------------------
// Returns the io transfer rate, in Megabytes/seconds of $SYSTEM.
//---------------------------------------------------------------------
float NAClusterInfoNSK::ioTransferRate() const
{
  return iorate_;
}

//---------------------------------------------------------------------
// Returns the average seek time of a disk device in milli seconds. 
//---------------------------------------------------------------------
float NAClusterInfoNSK::seekTime() const
{
  return seekTime_;
}

//--------------------------------------------------------------------
// Returns the procesor architecture in the current SMP
//--------------------------------------------------------------------
Int32 NAClusterInfoNSK::cpuArchitecture() const
{
  return CPU_ARCH_MIPS;
}

//---------------------------------------------------------------------
// Tells how many CPU's there are in the SMP
//---------------------------------------------------------------------
size_t NAClusterInfoNSK::numberOfCpusPerSMP() const
{
   return 1;
}

//---------------------------------------------------------------------
// Find the page size of the current SMP.
//---------------------------------------------------------------------
// LCOV_EXCL_START
size_t NAClusterInfoNSK::findPageSize()
{
  // should not come here on other platforms
  CMPASSERT(FALSE);
  return 0;
}
// LCOV_EXCL_STOP

//---------------------------------------------------------------------
// Returns the page size of the current SMP
//---------------------------------------------------------------------
size_t NAClusterInfoNSK::pageSize() const
{
   return pageSize_;
}

//---------------------------------------------------------------------
// Tells how much physical memory is available to our application
//---------------------------------------------------------------------
size_t NAClusterInfoNSK::physicalMemoryAvailable() const
{
   // Assumes the amount of physical memory is the same as that of total memory.
   return totalMemoryAvailable();
}

//---------------------------------------------------------------------
// Find out how much total memory exists on our machine.
//---------------------------------------------------------------------
// LCOV_EXCL_START
size_t NAClusterInfoNSK::findTotalMemoryAvailable()
{
  // should not come here on other platforms
  CMPASSERT(FALSE);
  return 0;

}
// LCOV_EXCL_STOP

//---------------------------------------------------------------------
// Tells how much physical memory exists on our machine
//---------------------------------------------------------------------
size_t NAClusterInfoNSK::totalMemoryAvailable() const
{
   return totalMemoryAvailable_;
}


//---------------------------------------------------------------------
// Tells how much virtual memory is available to our application
//---------------------------------------------------------------------
size_t NAClusterInfoNSK::virtualMemoryAvailable()
{
  return 128000000/1024;
}



NAClusterInfoLinux::NAClusterInfoLinux(CollHeap * heap) : NAClusterInfo(heap)
, numTSEs_(0), tseInfo_(NULL), nid_(0), pid_(0)
{
  OptimizerSimulator::osimMode mode = OptimizerSimulator::OFF;

  if(!CURRCONTEXT_OPTSIMULATOR->isCallDisabled(9))
    mode = CURRCONTEXT_OPTSIMULATOR->getOsimMode();

  // Check for OSIM mode
  switch (mode)
  {
    case OptimizerSimulator::OFF:
    case OptimizerSimulator::CAPTURE:
      determineLinuxSysInfo();

      // For CAPTURE mode, the data will be captured later in CmpMain::compile()
      break;
    case OptimizerSimulator::SIMULATE:
      // Simulate the NAClusterInfo.
      simulateNAClusterInfoLinux();
      break;
    // LCOV_EXCL_START
    default:
      // The OSIM must run under OFF (normal), CAPTURE or SIMULATE mode.
      OSIM_errorMessage("Invalid OSIM mode - It must be OFF or CAPTURE or SIMULATE.");
      break;
    // LCOV_EXCL_STOP
  }
}

NAClusterInfoLinux::~NAClusterInfoLinux()
{
   NADELETEBASIC(tseInfo_, heap_);
}

Int32 NAClusterInfoLinux::processorFrequency() const
{
  return frequency_;
}

float NAClusterInfoLinux::ioTransferRate() const
{
  return iorate_;
}

float NAClusterInfoLinux::seekTime() const
{
  return seekTime_;
}

Int32 NAClusterInfoLinux::cpuArchitecture() const
{
  return CPU_ARCH_UNKNOWN;
}

size_t NAClusterInfoLinux::numberOfCpusPerSMP() const
{
  return numCPUcoresPerNode_;
}

size_t NAClusterInfoLinux::pageSize() const
{
  return pageSize_;
}

// Return the physical memory available in kilobytes
size_t NAClusterInfoLinux::physicalMemoryAvailable() const
{
  // NSK returns the total memory available so we do the same thing
  // on Linux.  This allows the plans to stay constant even as
  // the amount of memory fluctuates.
  return totalMemoryAvailable_;
}

size_t NAClusterInfoLinux::totalMemoryAvailable() const
{
  return totalMemoryAvailable_;
}

size_t NAClusterInfoLinux::virtualMemoryAvailable()
{
  // Just return a constant (like NSK does).
  return 256000000/1024;
}

#define LINUX_DEFAULT_FREQ 3000
#define LINUX_IO_RATE  75.0
#define LINUX_SEEK_RATE 0.0038

void NAClusterInfoLinux::determineLinuxSysInfo()
{
  // Set the page size in killobytes and determine how much memory
  // is available on this node (in kilobytes).
  pageSize_ = (size_t)sysconf(_SC_PAGESIZE) / 1024U;
  totalMemoryAvailable_ = pageSize_ * (size_t)sysconf(_SC_PHYS_PAGES);
  numCPUcoresPerNode_ = sysconf(_SC_NPROCESSORS_ONLN);

  frequency_ = 0.0;

  // Read the CPU frequency from the sysfs filesystem.
  ifstream infoFile("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq");
  if (infoFile.fail()) {
    // This code should log a warning.

    // use /proc/cpuinfo
    char var[256];
    ifstream cpuInfoFile("/proc/cpuinfo");
    const char* freqToken = "cpu MHz";
    Lng32 freqTokenLen = strlen(freqToken);
    while(cpuInfoFile.good())
    {
      // Read the variable name from the file.
      cpuInfoFile.getline(var, sizeof(var), ':'); // read the token part
      Lng32 len = strlen(var);
      if(len >= freqTokenLen && !strncmp(var, freqToken, freqTokenLen))
      {
        cpuInfoFile >> frequency_;
        break;
      }
      cpuInfoFile.getline(var, sizeof(var)); // read the value part
    }

    if ( frequency_ == 0.0 )
       // Use the default frequency
       frequency_ = LINUX_DEFAULT_FREQ;
  } else {
    ULng32 freqUlongVal;
    infoFile >> freqUlongVal;
    frequency_ = freqUlongVal / 1000;
    infoFile.close();
  }

  // These should be determined programmatically, but are hard-coded for now.
  iorate_ = LINUX_IO_RATE;
  seekTime_ = LINUX_SEEK_RATE;
}

//============================================================================
// This method writes the information related to the NAClusterInfoLinux class
// to a logfile called "NAClusterInfo.txt".
//============================================================================
void NAClusterInfoLinux::captureOSInfo() const
{
  char filepath[OSIM_PATHMAX];
  char filename[OSIM_FNAMEMAX];

  strcpy(filepath, CURRCONTEXT_OPTSIMULATOR->getOsimLogdir());
  strcpy(filename, "/NAClusterInfo.txt");
  strcat(filepath, filename);
  ofstream nacllinuxfile(filepath, ios::app);

  nacllinuxfile << "frequency_: " << frequency_ << endl
                << "iorate_: " << iorate_ << endl
                << "seekTime_: "<< seekTime_ << endl
                << "pageSize_: " << pageSize_ << endl
                << "totalMemoryAvailable_: " << totalMemoryAvailable_ << endl
                << "numCPUcoresPerNode_: " << numCPUcoresPerNode_ << endl;
  nacllinuxfile.close();
}

void NAClusterInfoLinux::simulateNAClusterInfoLinux()
{
  char var[256];
  char filepath[OSIM_PATHMAX];
  char filename[OSIM_FNAMEMAX];

  strcpy(filepath, CURRCONTEXT_OPTSIMULATOR->getOsimLogdir());
  strcpy(filename, "/NAClusterInfo.txt");
  strcat(filepath, filename);
  NABoolean isDir;

  if(!fileExists(filepath,isDir))
  {
    char errMsg[38+OSIM_PATHMAX+1]; // Error msg below + filename + '\0'
   // LCOV_EXCL_START
    snprintf(errMsg, sizeof(errMsg), "Unable to open %s file for reading data.", filepath);
    OsimLogException(errMsg, __FILE__, __LINE__).throwException();
   // LCOV_EXCL_STOP
  }

  ifstream nacllinuxfile(filepath);

  while(nacllinuxfile.good())
  {
    // Read the variable name from the file
    nacllinuxfile.getline(var, sizeof(var), ':');
    if(!strcmp(var, "frequency_"))
    {
      nacllinuxfile >> frequency_; nacllinuxfile.ignore(OSIM_LINEMAX, '\n');
    }
    else if (!strcmp(var, "iorate_"))
    {
      nacllinuxfile >> iorate_; nacllinuxfile.ignore(OSIM_LINEMAX, '\n');
    }
    else if (!strcmp(var, "seekTime_"))
    {
      nacllinuxfile >> seekTime_; nacllinuxfile.ignore(OSIM_LINEMAX, '\n');
    }
    else if (!strcmp(var, "pageSize_"))
    {
      nacllinuxfile >> pageSize_; nacllinuxfile.ignore(OSIM_LINEMAX, '\n');
    }
    else if (!strcmp(var, "totalMemoryAvailable_"))
    {
      nacllinuxfile >> totalMemoryAvailable_; nacllinuxfile.ignore(OSIM_LINEMAX, '\n');
    }
    else if (!strcmp(var, "numCPUcoresPerNode_"))
    {
      nacllinuxfile >> numCPUcoresPerNode_; nacllinuxfile.ignore(OSIM_LINEMAX, '\n');
    }
    else
    {
      // This variable either may have been read in simulateNAClusterInfo()
      // method of NAClusterInfo class or is not the one that we want to
      // read here in this method. So discard it.
      nacllinuxfile.ignore(OSIM_LINEMAX, '\n');
      while (nacllinuxfile.peek() == ' ')
      {
        // The main variables are listed at the beginning of a line
        // with additional information indented. If one or more spaces
        // are seen at the beginning of the line upon the entry to this
        // while loop, it is because of that additional information.
        // So, ignore this line since the variable is being ignored.
        nacllinuxfile.ignore(OSIM_LINEMAX, '\n');
      }
    }
  }
}


Int32 compareTSEs( const void* a, const void* b ) 
{  
  // compare function
  MS_Mon_Process_Info_Type* arg1 = (MS_Mon_Process_Info_Type*) a;
  MS_Mon_Process_Info_Type* arg2 = (MS_Mon_Process_Info_Type*) b;

  if ( arg1->nid < arg2->nid )
    return -1;
  else  {
    if( arg1->nid == arg2->nid )
      return strcmp(arg1->process_name, arg2->process_name);
    else
     return 1;
  }
}

// setup TSE info for the POS. The method collects all TSEs in the cluster,
// filter out $SYSTEM, and sort the array in assending order on nid (node id).
// The method also can fake the number of TSEs when operated under POS test
// mode (cqd POS_TEST_MODE 'on'). In this special mode, the # of TSTs are 
// cqd POS_TEST_NUM_NODES times cqd POS_TEST_NUM_VOLUMES_PER_NODE.
// All faked TSEs are named numerically from 1 to # of TSTs. The test mode
// is for testing the disk_pool sub-feature.
void NAClusterInfoLinux::setupTSEinfoForPOS()
{
   if ( tseInfo_ ) {
      // LCOV_EXCL_START
      NADELETEBASIC(tseInfo_, heap_);
      tseInfo_ = NULL; numTSEs_ = 0;
      // LCOV_EXCL_STOP
   }

   short ret_val = msg_mon_get_process_info_type(MS_ProcessType_TSE,
                                             &numTSEs_,
                                             0,  // max ignored if info is NULL
                                             NULL);

   if ( ret_val != 0 ) 
     return;

   tseInfo_ = new (heap_) MS_Mon_Process_Info_Type [numTSEs_];

   ret_val = msg_mon_get_process_info_type(MS_ProcessType_TSE,
                                             &numTSEs_,
                                             numTSEs_,
                                             tseInfo_);


   if ( ret_val != 0 ) {
      // LCOV_EXCL_START
      NADELETEBASIC(tseInfo_, heap_);
      tseInfo_ = NULL; numTSEs_ = 0;
      return;
      // LCOV_EXCL_STOP
   }

   pid_ = getpid();

   for (Lng32 i= 0; i< numTSEs_; i++) {

     if ( tseInfo_[i].pid == pid_ )
       nid_ = tseInfo_[i].nid;

     // NOTE: The system metadata may be located in a volume other than
     //  $SYSTEM.  The following could change.  For now, skip any volumes
     //  called $SYSTEM.  The audit volumes aren't returned from the
     //  when MS_ProcessType_TSE is passed to msg_mon_get_process_info_type().
     //  Can add code here to filter out other TSEs if needed

     // here we replace a backup DP2 process or $SYSTEM process with the last
     // entry in the array in the hope that it is a good one.
     if (tseInfo_[i].backup != 0 ||
         strncmp(tseInfo_[i].process_name, "$SYSTEM", 7) == 0 )
     {
        if ( i < numTSEs_ - 1 ) {
          tseInfo_[i] = tseInfo_[numTSEs_-1]; // replace it with the
                                              // last entry from the array
          i--; // the previous last entry should be checked aginst backup and $system
               // because of process pairs
        }
        numTSEs_--;
     }
   }
  
#ifndef NDEBUG
   // LCOV_EXCL_START
   if (ActiveSchemaDB() && CmpCommon::getDefault(POS_TEST_MODE) == DF_ON) {
      NADefaults & defs = ActiveSchemaDB()->getDefaults();
      Int32 num_faked_nodes = (Int32)(defs.getAsLong(POS_TEST_NUM_NODES));
      Int32 num_faked_tses_per_node = 
            (Int32)(defs.getAsLong(POS_TEST_NUM_VOLUMES_PER_NODE));

      Int32 tses = 0;
      if (num_faked_nodes * num_faked_tses_per_node <= MAX_NUM_TSES) {
        for (Int32 i=0; i<num_faked_nodes; i++) {
           for (Int32 j=0; j<num_faked_tses_per_node; j++) {
              tseInfo_[tses].nid = i;

              char buf[20]; str_itoa(tses, buf);
              strcpy(tseInfo_[tses].process_name, buf);

              tses++;
           }
        }
        numTSEs_ = tses;
      }
   }
   // LCOV_EXCL_STOP
#endif
     
   qsort(tseInfo_, numTSEs_, sizeof(MS_Mon_Process_Info_Type), compareTSEs);
}


// get the TSE info for the kth volume (in the sorted order).
MS_Mon_Process_Info_Type* NAClusterInfoLinux::getTSEInfoForPOS(Int32 k)
{
   if ( tseInfo_ == NULL ) 
     setupTSEinfoForPOS();

   if ( k >= 0 && k < numTSEs_ )
     return &tseInfo_[k];
   else 
     return 0;
}

// get the total # of TSEs
Int32 NAClusterInfoLinux::numTSEsForPOS() 
{ 
   if ( tseInfo_ == NULL )
     setupTSEinfoForPOS();

   return numTSEs_; 
}


// LCOV_EXCL_START
NABoolean NAClusterInfo::IsRemoteNodeDown(short error)
{
  if ((error == FileSystemErrorRemoteNodeDown ) ||
      (error == FileSystemErrorRemoteNodeUnavailable) ||
      (error == FileSystemErrorNamedProcessNotInDCT) )
    return TRUE;
  else
    return FALSE;
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
const char * NAClusterInfo::GetNodeName(const char *dp2Name, char *buffer, Int32 size)
{
  strncpy(buffer, dp2Name, size);

  char *pos = buffer;

  Int32 i;
  for(i=0; i<size; i++){
    if(pos[i] == '.'){
      pos[i] = 0;
      break;
    }
  }

  DCMPASSERT(i<size);

  return buffer;
}
// LCOV_EXCL_STOP

NAList<CollIndex>*  maps::getCpuList(NABoolean aggregationNodeOnly)
{
   return (aggregationNodeOnly) ? listOfAggregationOnlyNodes : list;
}

Int32 maps::getCpuCount(NABoolean aggregationNodeOnly)
{
   return (aggregationNodeOnly) ? listOfAggregationOnlyNodes->entries() : 
                                  list->entries();
}


