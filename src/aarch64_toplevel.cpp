/*
 * toplevel.cpp
 *
 * Copyright (C) 2015, GreenSocs Ltd.
 *
 * Developed by Frederic Konrad <fred.konrad@greensocs.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses>.
 *
 * Linking GreenSocs code, statically or dynamically with other modules
 * is making a combined work based on GreenSocs code. Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * In addition, as a special exception, the copyright holders, GreenSocs
 * Ltd, give you permission to combine GreenSocs code with free software
 * programs or libraries that are released under the GNU LGPL, under the
 * OSCI license, under the OCP TLM Kit Research License Agreement or
 * under the OVP evaluation license.You may copy and distribute such a
 * system following the terms of the GNU GPL and the licenses of the
 * other code concerned.
 *
 * Note that people who make modified versions of GreenSocs code are not
 * obligated to grant this special exception for their modified versions;
 * it is their choice whether to do so. The GNU General Public License
 * gives permission to release a modified version without this exception;
 * this exception also makes it possible to release a modified version
 * which carries forward this exception.
 *
 */

// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

#include "SimpleCPU/simpleCPU.h"
#include "SimpleMemory/simpleMemory.h"
#include "nvdla/nvdla_top.h"
#include "log/log.h"

#include "greenrouter/genericRouter.h"
#include "greenrouter/protocol/SimpleBus/simpleBusProtocol.h"
#include "greenrouter/scheduler/fixedPriorityScheduler.h"
#include "greencontrol/config.h"
#include "greencontrol/config_api_lua_file_parser.h"

static void showUsage(std::string name)
{
    std::cerr << "Usage: " << name << " <option(s)>\n"
              << "Options:\n"
              << "\t-h,--help\t\t\tShow this help message\n"
              << "\t-c,--conf CONFIGURATION_FILE\tSpecify the configuration file path"
              << std::endl;
}

std::string parseArgs(int argc, char **argv) {
    std::string configurationFilePath;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            showUsage(argv[0]);
            exit(EXIT_SUCCESS);
        } else if ((arg == "-c") || (arg == "--conf")) {
            if (i + 1 < argc) {
                configurationFilePath = argv[++i];
            } else {
                std::cerr << "--conf option requires one argument." << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }
    return configurationFilePath;
}

std::string getSCLog(int argc, char **argv) {
    std::string sc_log_str;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-s") || (arg == "--sc_log")) {
            if (i + 1 < argc) {
                sc_log_str = argv[++i];
            } else {
                std::cerr << "--sc_log option requires one argument." << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }
    return sc_log_str;
}

int sc_main(int argc, char **argv)
{
  /* GSPARAMS */
  std::string configurationFilePath = parseArgs(argc, argv);
  gs::cnf::LuaFile_Tool luareader("luareader");
  bool openResult;
  if(configurationFilePath.empty()) {
    openResult = luareader.config("config.lua");
  } else {
    openResult = luareader.config(configurationFilePath.c_str());
  }
  
  if(openResult != 0) {
    showUsage(argv[0]);
    exit(EXIT_FAILURE);
  }

  char *SC_LOG = getenv("SC_LOG");
  if (SC_LOG != NULL) {
      string sc_log_str(SC_LOG);
      log_parse(sc_log_str);
  } else {
      std::string str_from_c = getSCLog(argc, argv);
      if (!str_from_c.empty()) {
          log_parse(str_from_c);
      } else {
          cout << "No sc_log specified, will use the default setting" << endl;
          cout << "verbosity_level = SC_MEDIUM" << endl;
      }
  }

  /*
   * CPU.
   */
  SimpleCPU *cpu = new SimpleCPU("CPU");

  /*
   * Memories.
   */
  Memory<32> *ram = new Memory<32>("ram");

  /*
   * Peripherals.
   */
  NVDLA_top *nvdla;
  nvdla = new NVDLA_top("nvdla");

  /*
   * Memory router.
   */
  gs::gp::SimpleBusProtocol<32> *protocol =
                            new gs::gp::SimpleBusProtocol<32>("protocol", 10);
  gs::gp::fixedPriorityScheduler *scheduler =
                            new gs::gp::fixedPriorityScheduler("scheduler");
  gs::gp::GenericRouter<32> *router =
                            new gs::gp::GenericRouter<32>("router");

  protocol->router_port(*router);
  protocol->scheduler_port(*scheduler);
  router->protocol_port(*protocol);

  /*
   * Bind the CPU.
   */
  cpu->master_socket(router->target_socket);

  /*
   * Bind the memories.
   */
  router->init_socket(ram->target_port);

  /*
   * Bind the nvdla.
   */
  router->init_socket(nvdla->target_port);
  nvdla->m_mc_port(router->target_socket);
  nvdla->m_cv_port(router->target_socket);
  nvdla->irq_socket(cpu->irq_socket);

  sc_core::sc_start();

  return 0;
}

