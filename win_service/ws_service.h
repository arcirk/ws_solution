// Service.h
//
// Author: Mikko Saarinki
// Copyright (c) 2016 Mikko Saarinki. All rights reserved.
//
#pragma once
#include <mutex>
#include <condition_variable>

#include <listener.hpp>
#include <shared_state.hpp>

#include <boost/asio/signal_set.hpp>
#include <boost/smart_ptr.hpp>
#include <iostream>
#include <vector>

#include <arcirk.h>
#include <string>

class ws_service
{
public:
    int
    RunServer();

    int run()
    {
        //std::unique_lock<std::mutex> lock(mtx);
        //sleeper.wait(lock, [this] { return !running; });
        std::thread tr(std::bind(&ws_service::RunServer, this));
        tr.join();
        return state();
    }

    void stop()
    {
        //std::lock_guard<std::mutex> lock(mtx);
        running = false;
        //sleeper.notify_all();
    }

    //@return 0 for A_OK and anything else for failures.
    int state() const { return EXIT_SUCCESS; }

private:
    //mutable std::mutex mtx;
    //std::condition_variable sleeper;
    bool running = true;
};

