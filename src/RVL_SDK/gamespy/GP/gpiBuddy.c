/*
gpiBuddy.c
GameSpy Presence SDK
Dan "Mr. Pants" Schoenblum

Copyright 1999-2007 GameSpy Industries, Inc

devsupport@gamespy.com

***********************************************************************
Please see the GameSpy Presence SDK documentation for more information
**********************************************************************/

// INCLUDES
//////////
#include "gpi.h"
#include <stdlib.h>
#include <string.h>

// FUNCTIONS
///////////
static GPResult gpiSendAuthBuddyRequest(GPConnection* connection,
                                        GPIProfile* profile) {
  GPIConnection* iconnection = (GPIConnection*)*connection;

  // Send the auth.
  /////////////////
  gpiAppendStringToBuffer(connection, &iconnection->outputBuffer,
                          "\\authadd\\");
  gpiAppendStringToBuffer(connection, &iconnection->outputBuffer,
                          "\\sesskey\\");
  gpiAppendIntToBuffer(connection, &iconnection->outputBuffer,
                       iconnection->sessKey);
  gpiAppendStringToBuffer(connection, &iconnection->outputBuffer,
                          "\\fromprofileid\\");
  gpiAppendIntToBuffer(connection, &iconnection->outputBuffer,
                       profile->profileId);
  gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, "\\sig\\");
  gpiAppendStringToBuffer(connection, &iconnection->outputBuffer,
                          profile->authSig);
  gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, "\\final\\");

  return GP_NO_ERROR;
}

GPResult gpiProcessRecvBuddyMessage(GPConnection* connection,
                                    const char* input) {
  char buffer[4096];
  int type;
  int profileid;
  time_t date;
  GPICallback callback;
  GPIProfile* profile;
  GPIBuddyStatus* buddyStatus;
  char intValue[16];
  char* str;
  unsigned short port;
  int productID;
  GPIConnection* iconnection = (GPIConnection*)*connection;
  char strTemp[max(GP_STATUS_STRING_LEN, GP_LOCATION_STRING_LEN)];

  // Check the type of bm.
  ////////////////////////
  if (!gpiValueForKey(input, "\\bm\\", buffer, sizeof(buffer)))
    CallbackFatalError(connection, GP_NETWORK_ERROR, GP_PARSE,
                       "Unexpected data was received from the server.");
  type = atoi(buffer);

  // Get the profile this is from.
  ////////////////////////////////
  if (!gpiValueForKey(input, "\\f\\", buffer, sizeof(buffer)))
    CallbackFatalError(connection, GP_NETWORK_ERROR, GP_PARSE,
                       "Unexpected data was received from the server.");
  profileid = atoi(buffer);

  // Get the time.
  ////////////////
  if (!gpiValueForKey(input, "\\date\\", buffer, sizeof(buffer)))
    date = time(NULL);
  else
    date = atoi(buffer);

  // What type of message is this?
  ////////////////////////////////
  switch (type) {
  case GPI_BM_MESSAGE:
    // Call the callback.
    /////////////////////
    callback = iconnection->callbacks[GPI_RECV_BUDDY_MESSAGE];
    if (callback.callback != NULL) {
      GPRecvBuddyMessageArg* arg;
      arg = (GPRecvBuddyMessageArg*)gsimalloc(sizeof(GPRecvBuddyMessageArg));
      if (arg == NULL)
        Error(connection, GP_MEMORY_ERROR, "Out of memory.");

      if (!gpiValueForKey(input, "\\msg\\", buffer, sizeof(buffer)))
        CallbackFatalError(connection, GP_NETWORK_ERROR, GP_PARSE,
                           "Unexpected data was received from the server.");
      arg->message = (char*)gsimalloc(strlen(buffer) + 1);
      if (arg->message == NULL)
        Error(connection, GP_MEMORY_ERROR, "Out of memory.");
      strcpy(arg->message, buffer);
      arg->profile = (GPProfile)profileid;
      arg->date = (unsigned int)date;
      CHECK_RESULT(
          gpiAddCallback(connection, callback, arg, NULL, GPI_ADD_MESSAGE));
    }
    break;
  case GPI_BM_UTM:
    // Call the callback.
    /////////////////////
    callback = iconnection->callbacks[GPI_RECV_BUDDY_UTM];
    if (callback.callback != NULL) {
      GPRecvBuddyUTMArg* arg;
      arg = (GPRecvBuddyUTMArg*)gsimalloc(sizeof(GPRecvBuddyUTMArg));
      if (arg == NULL)
        Error(connection, GP_MEMORY_ERROR, "Out of memory.");

      if (!gpiValueForKey(input, "\\msg\\", buffer, sizeof(buffer)))
        CallbackFatalError(connection, GP_NETWORK_ERROR, GP_PARSE,
                           "Unexpected data was received from the server.");
      arg->message = (char*)gsimalloc(strlen(buffer) + 1);
      if (arg->message == NULL)
        Error(connection, GP_MEMORY_ERROR, "Out of memory.");
      strcpy(arg->message, buffer);
      arg->profile = (GPProfile)profileid;
      arg->date = (unsigned int)date;
      CHECK_RESULT(
          gpiAddCallback(connection, callback, arg, NULL, GPI_ADD_BUDDYUTM));
    }
    break;

  case GPI_BM_REQUEST:
    // Get the profile, adding if needed.
    /////////////////////////////////////
    profile = gpiProfileListAdd(connection, profileid);
    if (!profile)
      Error(connection, GP_MEMORY_ERROR, "Out of memory.");

    // Get the reason.
    //////////////////
    if (!gpiValueForKey(input, "\\msg\\", buffer, sizeof(buffer)))
      CallbackFatalError(connection, GP_NETWORK_ERROR, GP_PARSE,
                         "Unexpected data was received from the server.");

    // Find where the sig starts.
    /////////////////////////////
    str = strstr(buffer, "|signed|");
    if (str == NULL)
      CallbackFatalError(connection, GP_NETWORK_ERROR, GP_PARSE,
                         "Unexpected data was received from the server.");

    // Get the sig out of the message.
    //////////////////////////////////
    *str = '\0';
    str += 8;
    if (strlen(str) != 32)
      CallbackFatalError(connection, GP_NETWORK_ERROR, GP_PARSE,
                         "Unexpected data was received from the server.");
    freeclear(profile->authSig);
    profile->authSig = goastrdup(str);
    profile->requestCount++;

    // Call the callback.
    /////////////////////
    callback = iconnection->callbacks[GPI_RECV_BUDDY_REQUEST];
    if (callback.callback != NULL) {
      GPRecvBuddyRequestArg* arg;
      arg = (GPRecvBuddyRequestArg*)gsimalloc(sizeof(GPRecvBuddyRequestArg));
      if (arg == NULL)
        Error(connection, GP_MEMORY_ERROR, "Out of memory.");
      strzcpy(arg->reason, buffer, GP_REASON_LEN);
      arg->profile = (GPProfile)profileid;
      arg->date = (unsigned int)date;

      CHECK_RESULT(gpiAddCallback(connection, callback, arg, NULL,
                                  GPI_ADD_BUDDDYREQUEST));
    }
    break;

  case GPI_BM_AUTH:
    // call the callback
    callback = iconnection->callbacks[GPI_RECV_BUDDY_AUTH];
    if (callback.callback != NULL) {
      GPRecvBuddyAuthArg* arg;
      arg = (GPRecvBuddyAuthArg*)gsimalloc(sizeof(GPRecvBuddyAuthArg));

      if (arg == NULL)
        Error(connection, GP_MEMORY_ERROR, "Out of memory.");
      arg->profile = (GPProfile)profileid;
      arg->date = (unsigned int)date;
      CHECK_RESULT(
          gpiAddCallback(connection, callback, arg, NULL, GPI_ADD_BUDDYAUTH));
    }
    break;

  case GPI_BM_REVOKE:
    // call the callback
    callback = iconnection->callbacks[GPI_RECV_BUDDY_REVOKE];
    if (callback.callback != NULL) {
      GPRecvBuddyRevokeArg* arg;
      arg = (GPRecvBuddyRevokeArg*)gsimalloc(sizeof(GPRecvBuddyRevokeArg));

      if (arg == NULL)
        Error(connection, GP_MEMORY_ERROR, "Out of memory.");
      arg->profile = (GPProfile)profileid;
      arg->date = (unsigned int)date;

      CHECK_RESULT(
          gpiAddCallback(connection, callback, arg, NULL, GPI_ADD_BUDDYREVOKE));
    }
    break;

  case GPI_BM_STATUS:
    // Get the profile, adding if needed.
    /////////////////////////////////////
    profile = gpiProfileListAdd(connection, profileid);
    if (!profile)
      Error(connection, GP_MEMORY_ERROR, "Out of memory.");

    // This is a buddy.
    ///////////////////
    if (!profile->buddyStatus) {
      profile->buddyStatus = (GPIBuddyStatus*)gsimalloc(sizeof(GPIBuddyStatus));
      if (!profile->buddyStatus)
        Error(connection, GP_MEMORY_ERROR, "Out of memory.");
      memset(profile->buddyStatus, 0, sizeof(GPIBuddyStatus));
      profile->buddyStatus->buddyIndex = iconnection->profileList.numBuddies++;
    }

    // Get the buddy status.
    ////////////////////////
    buddyStatus = profile->buddyStatus;

    // Get the msg.
    ///////////////
    if (!gpiValueForKey(input, "\\msg\\", buffer, sizeof(buffer)))
      CallbackFatalError(connection, GP_NETWORK_ERROR, GP_PARSE,
                         "Unexpected data was received from the server.");

    // Get the status.
    //////////////////
    if (!gpiValueForKey(buffer, "|s|", intValue, sizeof(intValue))) {
      CallbackFatalError(connection, GP_NETWORK_ERROR, GP_PARSE,
                         "Unexpected data was received from the server.");
    } else {
      buddyStatus->status = (GPEnum)atoi(intValue);
    }
    // Get the status string.
    /////////////////////////
    freeclear(buddyStatus->statusString);
    if (!gpiValueForKey(buffer, "|ss|", strTemp, GP_STATUS_STRING_LEN))
      strTemp[0] = '\0';
    buddyStatus->statusString = goastrdup(strTemp);
    if (!buddyStatus->statusString)
      Error(connection, GP_MEMORY_ERROR, "Out of memory.");

    // Get the location string.
    ///////////////////////////
    freeclear(buddyStatus->locationString);
    if (!gpiValueForKey(buffer, "|ls|", strTemp, GP_LOCATION_STRING_LEN))
      strTemp[0] = '\0';
    buddyStatus->locationString = goastrdup(strTemp);
    if (!buddyStatus->locationString)
      Error(connection, GP_MEMORY_ERROR, "Out of memory.");

    // Get the ip.
    //////////////
    if (!gpiValueForKey(buffer, "|ip|", intValue, sizeof(intValue)))
      buddyStatus->ip = 0;
    else
      buddyStatus->ip = htonl((unsigned int)atoi(intValue));

    // Get the port.
    ////////////////
    if (!gpiValueForKey(buffer, "|p|", intValue, sizeof(intValue)))
      buddyStatus->port = 0;
    else {
      port = (unsigned short)atoi(intValue);
      buddyStatus->port = htons(port);
    }

    // Call the callback.
    /////////////////////
    callback = iconnection->callbacks[GPI_RECV_BUDDY_STATUS];
    if (callback.callback != NULL) {
      GPRecvBuddyStatusArg* arg;
      arg = (GPRecvBuddyStatusArg*)gsimalloc(sizeof(GPRecvBuddyStatusArg));
      if (arg == NULL)
        Error(connection, GP_MEMORY_ERROR, "Out of memory.");

      arg->profile = (GPProfile)profileid;
      arg->index = buddyStatus->buddyIndex;
      arg->date = (unsigned int)date;

      CHECK_RESULT(
          gpiAddCallback(connection, callback, arg, NULL, GPI_ADD_STATUS));
    }
    break;

  case GPI_BM_INVITE:
    // Get the msg.
    ///////////////
    if (!gpiValueForKey(input, "\\msg\\", buffer, sizeof(buffer)))
      CallbackFatalError(connection, GP_NETWORK_ERROR, GP_PARSE,
                         "Unexpected data was received from the server.");

    // Find the productid.
    //////////////////////
    str = strstr(buffer, "|p|");
    if (str == NULL)
      CallbackFatalError(connection, GP_NETWORK_ERROR, GP_PARSE,
                         "Unexpected data was received from the server.");

    // Skip the |p|.
    ////////////////
    str += 3;
    if (str[0] == '\0')
      CallbackFatalError(connection, GP_NETWORK_ERROR, GP_PARSE,
                         "Unexpected data was received from the server.");

    // Get the productid.
    /////////////////////
    productID = atoi(str);

    // Find the location string (optional - older versions won't have)
    str = strstr(buffer, "|l|");
    if (str != NULL)
      strzcpy(strTemp, (str + 3), sizeof(strTemp));
    else
      strTemp[0] = '\0'; // no location, set to empty string

    // Call the callback.
    /////////////////////
    callback = iconnection->callbacks[GPI_RECV_GAME_INVITE];
    if (callback.callback != NULL) {
      GPRecvGameInviteArg* arg;
      arg = (GPRecvGameInviteArg*)gsimalloc(sizeof(GPRecvGameInviteArg));
      if (arg == NULL)
        Error(connection, GP_MEMORY_ERROR, "Out of memory.");

      arg->profile = (GPProfile)profileid;
      arg->productID = productID;
      strcpy(arg->location, strTemp);

      CHECK_RESULT(gpiAddCallback(connection, callback, arg, NULL, 0));
    }
    break;

  case GPI_BM_PING:
    // Get the msg.
    ///////////////
    if (!gpiValueForKey(input, "\\msg\\", buffer, sizeof(buffer)))
      CallbackFatalError(connection, GP_NETWORK_ERROR, GP_PARSE,
                         "Unexpected data was received from the server.");

    // Send back a pong.
    ////////////////////
    gpiSendBuddyMessage(connection, profileid, GPI_BM_PONG, "1", 0);

    break;

#ifndef NOFILE
  case GPI_BM_PONG:
    // Lets the transfers handle this.
    //////////////////////////////////
    gpiTransfersHandlePong(connection, profileid, NULL);

    break;
#endif
  }

  return GP_NO_ERROR;
}


GPResult gpiProcessRecvBuddyList(GPConnection* connection, const char* input) {
  int i = 0, j = 0;
  int num = 0;
  int index = 0;
  char c;
  char* str = NULL;
  char buffer[512];
  GPIProfile* profile;
  GPProfile profileid;
  GPIConnection* iconnection = (GPIConnection*)*connection;

  // Check for an error.
  //////////////////////
  if (gpiCheckForError(connection, input, GPITrue))
    return GP_SERVER_ERROR;

  // Process Buddy List Retrieval msg - Format like:
  /* ===============================================
     \bdy\<num in list>\list\<block list - comma delimited>\final\
     =============================================== */

  if (!gpiValueForKeyWithIndex(input, "\\bdy\\", &index, buffer,
                               sizeof(buffer)))
    CallbackFatalError(connection, GP_NETWORK_ERROR, GP_PARSE,
                       "Unexpected data was received from the server.");
  num = atoi(buffer);

  // Check to make sure list is there
  ///////////////////////////////////
  str = strstr(input, "\\list\\");
  if (str == NULL)
    CallbackFatalError(connection, GP_NETWORK_ERROR, GP_PARSE,
                       "Unexpected data was received from the server.");

  // Then increment index to get ready for parsing
  ////////////////////////////////////////////////
  str += 6;
  index += 6;

  for (i = 0; i < num; i++) {
    if (i == 0) {
      // Manually grab first profile in list - comma delimiter
      ////////////////////////////////////////////////////////
      for (j = 0; (j < sizeof(buffer)) && ((c = str[j]) != '\0') && (c != ',');
           j++) {
        buffer[j] = c;
      }
      buffer[j] = '\0';
      index += j;
    } else {
      if (!gpiValueForKeyWithIndex(input, ",", &index, buffer, sizeof(buffer)))
        CallbackFatalError(connection, GP_NETWORK_ERROR, GP_PARSE,
                           "Unexpected data was received from the server.");
    }

    profileid = atoi(buffer);

    // Get the profile, adding if needed.
    /////////////////////////////////////
    profile = gpiProfileListAdd(connection, profileid);
    if (!profile)
      Error(connection, GP_MEMORY_ERROR, "Out of memory.");

      // Mark as offline buddy for now until we get the real status
      /////////////////////////////////////////////////////////////
#ifdef GP_NEW_STATUS_INFO
    // Use new status info as placeholder
    profile->buddyStatusInfo =
        (GPIBuddyStatusInfo*)gsimalloc(sizeof(GPIBuddyStatusInfo));
    if (!profile->buddyStatusInfo)
      Error(connection, GP_MEMORY_ERROR, "Out of memory.");
    memset(profile->buddyStatusInfo, 0, sizeof(GPIBuddyStatusInfo));

    profile->buddyStatusInfo->extendedInfoKeys =
        ArrayNew(sizeof(GPIKey), GPI_INITIAL_NUM_KEYS, gpiStatusInfoKeyFree);
    if (!profile->buddyStatusInfo->extendedInfoKeys)
      Error(connection, GP_MEMORY_ERROR, "Out of memory.");

    profile->buddyStatusInfo->buddyIndex =
        iconnection->profileList.numBuddies++;
    profile->buddyStatusInfo->statusState = GP_OFFLINE;
#else
    // Use buddy status as placeholder
    profile->buddyStatus = (GPIBuddyStatus*)gsimalloc(sizeof(GPIBuddyStatus));
    if (!profile->buddyStatus)
      Error(connection, GP_MEMORY_ERROR, "Out of memory.");
    memset(profile->buddyStatus, 0, sizeof(GPIBuddyStatus));
    profile->buddyStatus->buddyIndex = iconnection->profileList.numBuddies++;
    profile->buddyStatus->status = GP_OFFLINE;
#endif
  }

  return GP_NO_ERROR;
}

GPResult gpiSendServerBuddyMessage(GPConnection* connection, int profileid,
                                   int type, const char* message) {
  char buffer[3501];
  GPIConnection* iconnection = (GPIConnection*)*connection;

  // Copy the message into an internal buffer.
  ////////////////////////////////////////////
  strzcpy(buffer, message, sizeof(buffer));

  // Setup the message.
  /////////////////////
  gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, "\\bm\\");
  gpiAppendIntToBuffer(connection, &iconnection->outputBuffer, type);
  gpiAppendStringToBuffer(connection, &iconnection->outputBuffer,
                          "\\sesskey\\");
  gpiAppendIntToBuffer(connection, &iconnection->outputBuffer,
                       iconnection->sessKey);
  gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, "\\t\\");
  gpiAppendIntToBuffer(connection, &iconnection->outputBuffer, profileid);
  gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, "\\msg\\");
  gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, buffer);
  gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, "\\final\\");

  return GP_NO_ERROR;
}

GPResult gpiSendBuddyMessage(GPConnection* connection, int profileid, int type,
                             const char* message, int sendOption) {
  GPIPeer* peer;
  GPIProfile* profile;
  // GPIConnection *iconnection = (GPIConnection *)*connection;
  peer = gpiGetPeerByProfile(connection, profileid);
  if (!peer) {
    // Check if we should send this through the server.
    ////////////////////////////////////////////////////
    if (!gpiGetProfile(connection, profileid, &profile) ||
        (!profile->buddyStatus || !profile->buddyStatus->port)) {
      if (sendOption == GP_DONT_ROUTE)
        return GP_NETWORK_ERROR;
      return gpiSendServerBuddyMessage(connection, profileid, type, message);
    }

    // Create a new peer connection for this message.
    /////////////////////////////////////////////////
    peer = gpiAddPeer(connection, profileid, GPITrue);
    if (!peer)
      return GP_MEMORY_ERROR;

    // Check if we need a sig.
    //////////////////////////
    if (!profile->peerSig) {
      // Get the sig.
      ///////////////
      CHECK_RESULT(gpiPeerGetSig(connection, peer));
    } else {
      // Try to connect to the peer.
      //////////////////////////////
      CHECK_RESULT(gpiPeerStartConnect(connection, peer));
    }
  }

  // Copy the message.
  ////////////////////
  CHECK_RESULT(gpiPeerAddMessage(connection, peer, type, message));

  return GP_NO_ERROR;
}



GPResult gpiAuthBuddyRequest(GPConnection* connection, GPProfile profile) {
  GPIProfile* pProfile;
  GPIConnection* iconnection = (GPIConnection*)*connection;

  // Get the profile object.
  //////////////////////////
  if (!gpiGetProfile(connection, profile, &pProfile))
    Error(connection, GP_PARAMETER_ERROR, "Invalid profile.");

  // Check for a valid sig.
  /////////////////////////
  if (!pProfile->authSig)
    Error(connection, GP_PARAMETER_ERROR, "Invalid profile.");

  // Send the request.
  ////////////////////
  CHECK_RESULT(gpiSendAuthBuddyRequest(connection, pProfile));

  // freeclear the sig if no more requests.
  ////////////////////////////////////
  pProfile->requestCount--;
  if (!iconnection->infoCaching && (pProfile->requestCount <= 0)) {
    freeclear(pProfile->authSig);
    if (gpiCanFreeProfile(pProfile))
      gpiRemoveProfile(connection, pProfile);
  }

  return GP_NO_ERROR;
}

GPIBool gpiFixBuddyIndices(GPConnection* connection, GPIProfile* profile,
                           void* data) {
  int baseIndex = (int)(unsigned long)data;

  GSI_UNUSED(connection);

  if (profile->buddyStatus && (profile->buddyStatus->buddyIndex > baseIndex))
    profile->buddyStatus->buddyIndex--;
  return GPITrue;
}

GPResult gpiDeleteBuddy(GPConnection* connection, GPProfile profile,
                        GPIBool sendServerRequest) {
  GPIProfile* pProfile;
  GPIConnection* iconnection = (GPIConnection*)*connection;
  int index;

  // Get the profile object.
  //////////////////////////
  if (!gpiGetProfile(connection, profile, &pProfile))
    Error(connection, GP_PARAMETER_ERROR, "Invalid profile.");

  // Check that this is a buddy.
  //////////////////////////////
  // Removed - 092404 BED - User could be a buddy even though we don't have the
  // status
  // if(!pProfile->buddyStatus)
  //	Error(connection, GP_PARAMETER_ERROR, "Profile not a buddy.");

  // Send the request.
  ////////////////////
  if (GPITrue == sendServerRequest) {
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer,
                            "\\delbuddy\\");
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer,
                            "\\sesskey\\");
    gpiAppendIntToBuffer(connection, &iconnection->outputBuffer,
                         iconnection->sessKey);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer,
                            "\\delprofileid\\");
    gpiAppendIntToBuffer(connection, &iconnection->outputBuffer,
                         pProfile->profileId);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer,
                            "\\final\\");
  }

  // Need to fix up the buddy indexes.
  ////////////////////////////////////
  if (pProfile->buddyStatus) {
    index = pProfile->buddyStatus->buddyIndex;
    assert(index >= 0);
    freeclear(pProfile->buddyStatus->statusString);
    freeclear(pProfile->buddyStatus->locationString);
    freeclear(pProfile->buddyStatus);
    if (gpiCanFreeProfile(pProfile))
      gpiRemoveProfile(connection, pProfile);
    iconnection->profileList.numBuddies--;
    assert(iconnection->profileList.numBuddies >= 0);
    gpiProfileMap(connection, gpiFixBuddyIndices, (void*)(unsigned long)index);
  }
  return GP_NO_ERROR;
}
