// Copyright 2022 locallocal
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "ccmd.h"

std::shared_ptr<ccmd::command> register_commands();
void add_master_command(std::shared_ptr<ccmd::command> cmd);
void add_meta_command(std::shared_ptr<ccmd::command> cmd);
void add_storage_command(std::shared_ptr<ccmd::command> cmd);
void add_replication_command(std::shared_ptr<ccmd::command> cmd);
void root_run(std::shared_ptr<ccmd::command> cmd);
void master_run(std::shared_ptr<ccmd::command> cmd);
void meta_run(std::shared_ptr<ccmd::command> cmd);
void storage_run(std::shared_ptr<ccmd::command> cmd);
void replication_run(std::shared_ptr<ccmd::command> cmd);

int main(int argc, char* argv[]) {
    std::shared_ptr<ccmd::command> root_cmd = register_commands();
    root_cmd->execute(argc, argv);
    return 0;
}

std::shared_ptr<ccmd::command> register_commands() {
    std::shared_ptr<ccmd::command> root_cmd = std::make_shared<ccmd::command>(
        /* name       */ "example",
        /* example    */ "example meta [options].",
        /* usage      */ "example [subcommand] [options].",
        /* help_long  */ "example is a tutorial of ccmd.",
        /* help_short */ "example command.",
        /* run        */ root_run);
    root_cmd->varp<bool>("version", "v", false, "show version.");
    root_cmd->var<bool>("verbose", false, "show verbose.");
    // A long option name with a description longer than one help line: the
    // help printer wraps it and keeps the continuation lines aligned.
    root_cmd->var<std::string>(
        "log-output-directory", "/var/log/example",
        "directory that receives the rotated log files of every server started from this binary; it is "
        "created on first use, must be writable by the service user, and is shared by all subcommands.");

    add_master_command(root_cmd);
    add_meta_command(root_cmd);
    add_storage_command(root_cmd);
    add_replication_command(root_cmd);
    return root_cmd;
}

void root_run(std::shared_ptr<ccmd::command> cmd) { std::cout << "This is the ccmd example." << std::endl; }

void add_master_command(std::shared_ptr<ccmd::command> cmd) {
    std::shared_ptr<ccmd::command> master_cmd = std::make_shared<ccmd::command>(
        /* name       */ "master",
        /* example    */ "example master --conf=./master.conf --port=9999.",
        /* usage      */ "master [--conf=./master.conf] [--port=port] [-d/--daemon].",
        /* help_long  */ "start a master server.",
        /* help_short */ "start a master server.",
        /* run        */ master_run);
    master_cmd->varp<bool>("daemon", "d", false, "run daemon.");
    master_cmd->varp<int>("port", "p", 9999, "set server port.");
    master_cmd->varp<std::string>("conf", "c", "", "configuration file.");
    cmd->add_subcommand(master_cmd);
}

void add_meta_command(std::shared_ptr<ccmd::command> cmd) {
    std::shared_ptr<ccmd::command> meta_cmd = std::make_shared<ccmd::command>(
        /* name       */ "meta",
        /* example    */ "example meta --conf=./meta.conf --port=10000.",
        /* usage      */ "meta [--master=127.0.0.1:9999] [--conf=./meta.conf] [--port=port] [-d/--daemon].",
        /* help_long  */ "start a meta server.",
        /* help_short */ "start a meta server.",
        /* run        */ meta_run);
    meta_cmd->varp<bool>("daemon", "d", false, "run daemon.");
    meta_cmd->varp<int>("port", "p", 9998, "set server port.");
    meta_cmd->varp<std::string>("master", "m", "0.0.0.0", "master address.");
    meta_cmd->varp<std::string>("conf", "c", "./meta.conf", "configuration file.");
    cmd->add_subcommand(meta_cmd);
}

void add_storage_command(std::shared_ptr<ccmd::command> cmd) {
    std::shared_ptr<ccmd::command> storage_cmd = std::make_shared<ccmd::command>(
        /* name       */ "storage",
        /* example    */ "example storage --conf=./storage.conf --port=10001.",
        /* usage      */ "storage [--master=127.0.0.1:9999] [--conf=./storage.conf] [--port=port] [-d/--daemon].",
        /* help_long  */ "start a storage server.",
        /* help_short */ "start a storage server.",
        /* run        */ storage_run);
    storage_cmd->varp<bool>("daemon", "d", false, "run daemon.");
    storage_cmd->varp<int>("port", "p", 9997, "set server port.");
    storage_cmd->varp<std::string>("master", "m", "0.0.0.0", "master address.");
    storage_cmd->varp<std::string>("conf", "c", "./storage.conf", "configuration file.");
    cmd->add_subcommand(storage_cmd);
}

// A subcommand whose name, short description and option descriptions are all
// longer than usual, so that `example --help` and
// `example replication-controller --help` show wrapped, aligned output.
void add_replication_command(std::shared_ptr<ccmd::command> cmd) {
    std::shared_ptr<ccmd::command> replication_cmd = std::make_shared<ccmd::command>(
        /* name       */ "replication-controller",
        /* example    */
        "example replication-controller --master=127.0.0.1:9999 "
        "--heartbeat-interval-seconds=5 --max-inflight-replication-requests=64.",
        /* usage      */
        "replication-controller [--master=host:port] [--heartbeat-interval-seconds=seconds] "
        "[--max-inflight-replication-requests=count] [--snapshot-directory=dir] [-d/--daemon].",
        /* help_long  */
        "start the replication controller, which keeps every storage replica in sync with the "
        "master by streaming committed writes, verifying checksums, and rebuilding replicas from "
        "the latest snapshot when they fall too far behind to catch up incrementally.",
        /* help_short */
        "start the replication controller that keeps storage replicas in sync with the master "
        "and rebuilds them from snapshots when needed.",
        /* run        */ replication_run);
    replication_cmd->varp<bool>("daemon", "d", false, "run daemon.");
    replication_cmd->varp<std::string>("master", "m", "0.0.0.0:9999", "master address.");
    replication_cmd->var<int>(
        "heartbeat-interval-seconds", 10,
        "seconds between two heartbeats sent to every replica; a replica that misses three consecutive "
        "heartbeats is marked as lagging and is scheduled for a full rebuild from the latest snapshot.");
    replication_cmd->var<int>(
        "max-inflight-replication-requests", 32,
        "upper bound on replication requests that may be in flight at the same time across all replicas; "
        "raise it on fast networks to increase throughput, lower it to reduce memory pressure on the master.");
    replication_cmd->var<std::string>("snapshot-directory", "./snapshots",
                                      "directory that holds the snapshots used to rebuild lagging replicas.");
    cmd->add_subcommand(replication_cmd);
}

void master_run(std::shared_ptr<ccmd::command> cmd) {
    for (auto& arg : cmd->args()) {
        std::cout << arg << " ";
    }
    std::cout << std::endl;
    std::cout << "daemon: " << std::boolalpha << cmd->var<bool>("daemon") << std::endl;
    std::cout << "port: " << cmd->var<int>("port") << std::endl;
    std::cout << "conf: " << cmd->var<std::string>("conf") << std::endl;
}

void meta_run(std::shared_ptr<ccmd::command> cmd) {
    for (auto& arg : cmd->args()) {
        std::cout << arg << " ";
    }
    std::cout << std::endl;
    std::cout << "daemon: " << std::boolalpha << cmd->var<bool>("daemon") << std::endl;
    std::cout << "port: " << cmd->var<int>("port") << std::endl;
    std::cout << "master: " << cmd->var<std::string>("master") << std::endl;
    std::cout << "conf: " << cmd->var<std::string>("conf") << std::endl;
}

void storage_run(std::shared_ptr<ccmd::command> cmd) {
    for (auto& arg : cmd->args()) {
        std::cout << arg << " ";
    }
    std::cout << std::endl;
    std::cout << "daemon: " << std::boolalpha << cmd->var<bool>("daemon") << std::endl;
    std::cout << "port: " << cmd->var<int>("port") << std::endl;
    std::cout << "master: " << cmd->var<std::string>("master") << std::endl;
    std::cout << "conf: " << cmd->var<std::string>("conf") << std::endl;
}

void replication_run(std::shared_ptr<ccmd::command> cmd) {
    for (auto& arg : cmd->args()) {
        std::cout << arg << " ";
    }
    std::cout << std::endl;
    std::cout << "daemon: " << std::boolalpha << cmd->var<bool>("daemon") << std::endl;
    std::cout << "master: " << cmd->var<std::string>("master") << std::endl;
    std::cout << "heartbeat-interval-seconds: " << cmd->var<int>("heartbeat-interval-seconds") << std::endl;
    std::cout << "max-inflight-replication-requests: " << cmd->var<int>("max-inflight-replication-requests")
              << std::endl;
    std::cout << "snapshot-directory: " << cmd->var<std::string>("snapshot-directory") << std::endl;
}
