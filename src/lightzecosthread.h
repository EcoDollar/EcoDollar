// Copyright (c) 2015-2019 The ECODOLLAR developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
//

#ifndef ECODOLLAR_LIGHTZECOSTHREAD_H
#define ECODOLLAR_LIGHTZECOSTHREAD_H

#include <atomic>
#include "genwit.h"
#include "zecos/accumulators.h"
#include "concurrentqueue.h"
#include "chainparams.h"
#include <boost/function.hpp>
#include <boost/thread.hpp>

extern CChain chainActive;
// Max amount of computation for a single request
const int COMP_MAX_AMOUNT = 60 * 24 * 60;


/****** Thread ********/

class CLightWorker{

private:

    concurrentqueue<CGenWit> requestsQueue;
    std::atomic<bool> isWorkerRunning;
    boost::thread threadIns;

public:

    CLightWorker() {
        isWorkerRunning = false;
    }

    enum ERROR_CODES {
        NOT_ENOUGH_MINTS = 0,
        NON_DETERMINED = 1
    };

    bool addWitWork(CGenWit wit) {
        if (!isWorkerRunning) {
            LogPrintf("%s not running trying to add wit work \n", "ecodollar-light-thread");
            return false;
        }
        requestsQueue.push(wit);
        return true;
    }

    void StartLightZecosThread(boost::thread_group& threadGroup) {
        LogPrintf("%s thread start\n", "ecodollar-light-thread");
        threadIns = boost::thread(boost::bind(&CLightWorker::ThreadLightZECOSSimplified, this));
    }

    void StopLightZecosThread() {
        threadIns.interrupt();
        LogPrintf("%s thread interrupted\n", "ecodollar-light-thread");
    }

private:

    void ThreadLightZECOSSimplified();

    void rejectWork(CGenWit& wit, int blockHeight, uint32_t errorNumber);

};

#endif //ECODOLLAR_LIGHTZECOSTHREAD_H
