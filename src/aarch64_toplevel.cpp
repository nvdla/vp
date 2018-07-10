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

#include <pthread.h>

#include "greenrouter/genericRouter.h"
#include "greenrouter/protocol/SimpleBus/simpleBusProtocol.h"
#include "greenrouter/scheduler/fixedPriorityScheduler.h"
#include "greencontrol/config.h"
#include "greencontrol/config_api_lua_file_parser.h"

#if AWS_FPGA_PRESENT
#include <string>
#include <fcntl.h>
#include "utils/log.h"
#include "fpga_pci.h"
#include "fpga_mgmt.h"
#include "fpga_sc_wrapper/nvdla_fpga_sc_wrapper.h"
#include "fpga_sc_wrapper/extmem_fpga_sc_wrapper.h"

#define MAX_NUM_INT 16

typedef struct {
    int slot_id;
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t msi_mask;
} aws_fpga_info_t;

typedef struct {
    pci_bar_handle_t pci_bar_handle_ocl;
    pci_bar_handle_t pci_bar_handle_pcis;
    pci_bar_handle_t pci_bar_handle_bar1;
    int pci_msi_fd[MAX_NUM_INT];
} aws_fpga_handle_t;

const struct logger *logger = &logger_stdout;

static int check_afi_ready(const aws_fpga_info_t &aws_fpga_info);
static int aws_fpga_init(const aws_fpga_info_t &aws_fpga_info, aws_fpga_handle_t &aws_fpga_handle);
static int aws_fpga_cleanup(const aws_fpga_handle_t &aws_fpga_handle);
#endif

#include "cosim_sc_wrapper/nvdla_cosim_sc_wrapper.h"
#include "cosim_sc_wrapper/extmem_cosim_sc_wrapper.h"
#include "stdlib.h"
#include "unistd.h"

// Mutex for Qemu memory access DMI mode which will optimize the platform performance
static pthread_mutex_t dmi_mtx;

static void showUsage(std::string name)
{
    std::cerr << "Usage: " << name << " <option(s)>\n"
              << "Options:\n"
              << "\t-h,--help\t\t\tShow this help message\n"
              << "\t-c,--conf CONFIGURATION_FILE\tSpecify the configuration file path"
              << "\t--cosim\tRunning with QEMU/RTL co-simulation"
              << "\t--simv\tRTL simv file path. Only required with --cosim option"
              << "\t--timeout\tQemu wait timeout (simulation time in ns) with vcs. Only required with --cosim option"
              << "\t--plusargs\trtl simulation args. Only required with --cosim option"
#if AWS_FPGA_PRESENT
              << "\t--fpga\tRunning with FPGA"
              << "\t--slot_id\tFPGA slot id"
              << "\t--vender_id\tFPGA vender id"
              << "\t--device_id\tFPGA device id"
              << "\t--msi_mask\tFPGA msi mask"
#endif
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

std::string parseSimvArgs(int argc, char **argv) {
    std::string simvFilePath;
    std::string timeout_ns;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "--simv")) {
            if (i + 1 < argc) {
                simvFilePath = argv[++i];
            } else {
                std::cerr << "--simv option requires one argument." << std::endl;
                exit(EXIT_FAILURE);
            }
        }

        if ((arg == "--timeout")) {
            if (i + 1 < argc) {
                timeout_ns = argv[++i];
            } else {
                std::cerr << "--timeout option requires one argument." << std::endl;
                exit(EXIT_FAILURE);
            }
            setenv("COSIM_SHM_TIMEOUT_IN_NS", timeout_ns.c_str(), 1);
        }
    }
    return simvFilePath;
}

std::string parsePlusArgsArgs(int argc, char **argv)
{
    std::string plusargs_str;
     for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "--plusargs")) {
            if (i + 1 < argc) {
                plusargs_str = argv[++i];
            } else {
                std::cerr << "--plusargs option requires one argument." << std::endl;
                exit(EXIT_FAILURE);
            }
            setenv("COSIM_PLUS_ARGS", plusargs_str.c_str(), 1);
        }
    }

    return plusargs_str;
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

bool parseCosimArgs(int argc, char **argv) {
    bool cosim_en = 0;
    bool simv = 0;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--cosim") {
            cosim_en = 1;
        }
        if (arg == "--simv") {
            simv = 1;
        }
    }

    if ((simv == 0) && (cosim_en == 1))
    {
        std::cerr << "Need to set simv file path with --simv option" << std::endl;
        exit(EXIT_FAILURE);
    }

    return cosim_en;
}

bool parseDmiArgs(int argc, char **argv) {
    bool dmi_en = true; // set dmi as default
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--dmi") {
            dmi_en = true;
        }
        else if(arg == "--dmi_disable") {
            dmi_en = false;
        }
    }

    return dmi_en;
}



#if AWS_FPGA_PRESENT
bool parseFpgaArgs(int argc, char **argv, aws_fpga_info_t &aws_fpga_info) {
    bool aws_fpga_en = 0;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--fpga") {
            aws_fpga_en = 1;
        } else if (arg == "--slot_id") {
            if (i + 1 < argc) {
                aws_fpga_info.slot_id = std::stoi(argv[++i], nullptr, 0);
            } else {
                std::cerr << "--slot_id option requires one argument." << std::endl;
                exit(EXIT_FAILURE);
            }
        } else if (arg == "--vendor_id") {
            if (i + 1 < argc) {
                aws_fpga_info.vendor_id = std::stoi(argv[++i], nullptr, 0);
            } else {
                std::cerr << "--vendor_id option requires one argument." << std::endl;
                exit(EXIT_FAILURE);
            }
        } else if (arg == "--device_id") {
            if (i + 1 < argc) {
                aws_fpga_info.device_id = std::stoi(argv[++i], nullptr, 0);
            } else {
                std::cerr << "--device_id option requires one argument." << std::endl;
                exit(EXIT_FAILURE);
            }
        } else if (arg == "--msi_mask") {
            if (i + 1 < argc) {
                aws_fpga_info.msi_mask = std::stoi(argv[++i], nullptr, 0);
            } else {
                std::cerr << "--msi_mask option requires one argument." << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }
    return aws_fpga_en;
}
#endif
#if AWS_FPGA_PRESENT
#else
static void exec_simv(const char* simv_path, const char* plusargs_c)
{
    pid_t nPID      = -1;
    pid_t nPID_host = getpid();

    std::stringstream pid_str;
    pid_str << nPID_host;
    setenv("COSIM_PID_HOST", pid_str.str().c_str(), 1);
    std::stringstream simv_str;

    simv_str << "./simv -l cosim_test.log +ntb_random_seed_automatic +vpdfile+cosim_test.vpd " << plusargs_c;

    char apath[2048];
    char *ptr;
    ptr = realpath(simv_path, apath);
    if (ptr != NULL)
    {
        std::string cmd = "/bin/bash";

        nPID = fork();
        if (nPID < 0) { 
            throw std::runtime_error("failed to fork simv");
        } else if(nPID == 0) {
            // exec simv if child
            cout << "Inside child process" << endl;
            if (chdir(apath) == -1)
            {
                cerr << "chdir failed - " << strerror (errno) << endl;
                return;
            }
            char *arg[] = {const_cast<char *>(cmd.c_str()), const_cast<char *>(string("-c").c_str()), const_cast<char *>(simv_str.str().c_str()), (char *)0};
            extern char **environ;
            execve(arg[0], arg, environ);
            return;
        } else {
            cout << "Inside parent process" << endl;
        }
    } else {
        std::cerr << "The path is not exist: " << simv_path << std::endl;
        exit(EXIT_FAILURE);
    }
}
#endif

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

#if AWS_FPGA_PRESENT
    /* Initialize AWS_FPGA */
    bool aws_fpga_en;
    aws_fpga_info_t aws_fpga_info = {0, 0x1D0F, 0xF001, 0x1};
    aws_fpga_handle_t aws_fpga_handle;
    aws_fpga_en = parseFpgaArgs(argc, argv, aws_fpga_info);
    if (aws_fpga_en) {
        std::cout << "Initialize AWS FPGA with slot_id=" << aws_fpga_info.slot_id << ", pci_vendor_id=0x" << std::hex << aws_fpga_info.vendor_id << ", pci_device_id=0x" << aws_fpga_info.device_id << std::endl;
        if (aws_fpga_init(aws_fpga_info, aws_fpga_handle)) {
            std::cerr << "Error when initialize the AWS FPGA." << std::endl;
            exit(EXIT_FAILURE);
        }
    }
#endif

    bool cosim_en;
    cosim_en = parseCosimArgs(argc, argv);

    std::string simv_path_str;
    std::string rtl_plusargs_str;
    if (cosim_en) {
        simv_path_str    = parseSimvArgs(argc, argv);
        rtl_plusargs_str = parsePlusArgsArgs(argc, argv);
    }

    bool dmi_en;
    dmi_en = parseDmiArgs(argc, argv);


    if (dmi_en)
    {
        pthread_mutex_init(&dmi_mtx, NULL);
        cout << "DMI mode enable"  << endl;
    } else {
        cout << "DMI mode disable" << endl;
    }

    /*
     * CPU.
     */
    SimpleCPU *cpu = new SimpleCPU("CPU");

    if (dmi_en){
#if AWS_FPGA_PRESENT
        if (aws_fpga_en){
            cpu->set_dmi_mutex(&dmi_mtx, aws_fpga_en);//set the dmi en and mutex for simpleCPU with fpga
            cpu->set_pci_bar_handle(aws_fpga_handle.pci_bar_handle_pcis); //Pass the AWS FPGA pci_bar_handle_pcis to Simple CPU; then it can access the memory directly.
        } else {
            cpu->set_dmi_mutex(&dmi_mtx, aws_fpga_en);// set the mutex for cmod
        }
#else
        cpu->set_dmi_mutex(&dmi_mtx);
#endif
    }
    /*
     * Memories.
     */
#if AWS_FPGA_PRESENT
    void *ram;
    if (aws_fpga_en) {
        ram = new extmem_fpga_sc_wrapper("ram", aws_fpga_handle.pci_bar_handle_pcis);
        if(dmi_en) {
            cpu->set_dmi_base_addr(((extmem_fpga_sc_wrapper *)ram)->target_port.base_addr); //Set the DMI base_addr
        }
    } else {
        ram = new Memory<32>("ram");
        if (dmi_en)
            cpu->set_dmi_base_addr(((Memory<32> *)ram)->target_port.base_addr);
    }
#else
void *ram;
if (cosim_en) {
    ram = new extmem_cosim_sc_wrapper("ram");
} else {
    ram = new Memory<32>("ram");
    if (dmi_en)
        cpu->set_dmi_base_addr(((Memory<32> *)ram)->target_port.base_addr);
}
#endif

    /*
     * Peripherals.
     */
#if AWS_FPGA_PRESENT
    void *nvdla;
    if (aws_fpga_en) {
        nvdla = new nvdla_fpga_sc_wrapper("nvdla", aws_fpga_handle.pci_bar_handle_ocl, aws_fpga_handle.pci_bar_handle_bar1, aws_fpga_handle.pci_msi_fd[0]);
    } else {
        nvdla = new NVDLA_top("nvdla");
        if (dmi_en)
            ((NVDLA_top*)nvdla)->set_dmi_mutex(&dmi_mtx);
    }
#else
void *nvdla;
if (cosim_en) {
    nvdla = new nvdla_cosim_sc_wrapper("nvdla");
} else {
    nvdla = new NVDLA_top("nvdla");
    if (dmi_en)
        ((NVDLA_top*)nvdla)->set_dmi_mutex(&dmi_mtx);
}
#endif

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
#if AWS_FPGA_PRESENT
    if (aws_fpga_en) {
        router->init_socket((reinterpret_cast<extmem_fpga_sc_wrapper *>(ram))->target_port);
    } else {
        router->init_socket((reinterpret_cast<Memory<32> *>(ram))->target_port);
    }
#else
if (cosim_en) {
    router->init_socket((reinterpret_cast<extmem_cosim_sc_wrapper *>(ram))->target_port);
} else {
    router->init_socket((reinterpret_cast<Memory<32> *>(ram))->target_port);
}
#endif

    /*
     * Bind the nvdla.
     */
#if AWS_FPGA_PRESENT
    if (aws_fpga_en) {
        router->init_socket((reinterpret_cast<nvdla_fpga_sc_wrapper *>(nvdla))->target_port);
        (reinterpret_cast<nvdla_fpga_sc_wrapper *>(nvdla))->irq_socket(cpu->irq_socket);
    } else {
        router->init_socket((reinterpret_cast<NVDLA_top *>(nvdla))->target_port);
        (reinterpret_cast<NVDLA_top *>(nvdla))->m_mc_port(router->target_socket);
        (reinterpret_cast<NVDLA_top *>(nvdla))->m_cv_port(router->target_socket);
        (reinterpret_cast<NVDLA_top *>(nvdla))->irq_socket(cpu->irq_socket);
    }
#else
if (cosim_en) {
    router->init_socket((reinterpret_cast<nvdla_cosim_sc_wrapper *>(nvdla))->target_port);
    (reinterpret_cast<nvdla_cosim_sc_wrapper *>(nvdla))->irq_socket(cpu->irq_socket);

    exec_simv(simv_path_str.c_str(), rtl_plusargs_str.c_str());
} else {
    router->init_socket((reinterpret_cast<NVDLA_top *>(nvdla))->target_port);
    (reinterpret_cast<NVDLA_top *>(nvdla))->m_mc_port(router->target_socket);
    (reinterpret_cast<NVDLA_top *>(nvdla))->m_cv_port(router->target_socket);
    (reinterpret_cast<NVDLA_top *>(nvdla))->irq_socket(cpu->irq_socket);
}
#endif

    sc_core::sc_start();

#if AWS_FPGA_PRESENT
    if (aws_fpga_en) {
        if (aws_fpga_cleanup(aws_fpga_handle)) {
            std::cerr << "Error when cleanup the AWS FPGA." << std::endl;
            exit(EXIT_FAILURE);
        }
    }
#endif

    return 0;
}

#if AWS_FPGA_PRESENT
/* check if the corresponding AFI is loaded */
static int check_afi_ready(const aws_fpga_info_t &aws_fpga_info)
{
    struct fpga_mgmt_image_info info = {0}; 
    int rc;

    /* get local image description, contains status, vendor id, and device id. */
    rc = fpga_mgmt_describe_local_image(aws_fpga_info.slot_id, &info,0);
    fail_on(rc, out, "Unable to get AFI information from slot %d. Are you running as root?",aws_fpga_info.slot_id);

    /* check to see if the slot is ready */
    if (info.status != FPGA_STATUS_LOADED) {
        rc = 1;
        fail_on(rc, out, "AFI in Slot %d is not in READY state !", aws_fpga_info.slot_id);
    }

    printf("AFI PCI  Vendor ID: 0x%x, Device ID 0x%x\n",
        info.spec.map[FPGA_APP_PF].vendor_id,
        info.spec.map[FPGA_APP_PF].device_id);

    /* confirm that the AFI that we expect is in fact loaded */
    if (info.spec.map[FPGA_APP_PF].vendor_id != aws_fpga_info.vendor_id ||
        info.spec.map[FPGA_APP_PF].device_id != aws_fpga_info.device_id) {
        printf("AFI does not show expected PCI vendor id and device ID. If the AFI "
               "was just loaded, it might need a rescan. Rescanning now.\n");

        rc = fpga_pci_rescan_slot_app_pfs(aws_fpga_info.slot_id);
        fail_on(rc, out, "Unable to update PF for slot %d",aws_fpga_info.slot_id);
        /* get local image description, contains status, vendor id, and device id. */
        rc = fpga_mgmt_describe_local_image(aws_fpga_info.slot_id, &info,0);
        fail_on(rc, out, "Unable to get AFI information from slot %d",aws_fpga_info.slot_id);

        printf("AFI PCI  Vendor ID: 0x%x, Device ID 0x%x\n",
            info.spec.map[FPGA_APP_PF].vendor_id,
            info.spec.map[FPGA_APP_PF].device_id);

        /* confirm that the AFI that we expect is in fact loaded after rescan */
        if (info.spec.map[FPGA_APP_PF].vendor_id != aws_fpga_info.vendor_id ||
             info.spec.map[FPGA_APP_PF].device_id != aws_fpga_info.device_id) {
            rc = 1;
            fail_on(rc, out, "The PCI vendor id and device of the loaded AFI are not "
                             "the expected values.");
        }
    }

    return (rc != 0 ? 1: 0);
out:
    return 1;
}

static int aws_fpga_init(const aws_fpga_info_t &aws_fpga_info, aws_fpga_handle_t &aws_fpga_handle)
{
    int rc;
    int pf_id;
    int bar_id;
    
    /* setup logging to print to stdout */
    rc = log_init("nvdla");
    fail_on(rc, out, "Unable to initialize the log.");
    rc = log_attach(logger, NULL, 0);
    fail_on(rc, out, "%s", "Unable to attach to the log.");

    /* initialize the fpga_pci library so we could have access to FPGA PCIe from this applications */
    rc = fpga_pci_init();
    fail_on(rc, out, "Unable to initialize the fpga_pci library");

    rc = check_afi_ready(aws_fpga_info);
    fail_on(rc, out, "AFI not ready");

    /* initialize the bar handle */
    pf_id = FPGA_APP_PF;
    
    bar_id = APP_PF_BAR0;
    rc = fpga_pci_attach(aws_fpga_info.slot_id, pf_id, bar_id, 0, &aws_fpga_handle.pci_bar_handle_ocl);
    fail_on(rc, out, "Unable to attach to the AFI on slot id %d, bar_id %d", aws_fpga_info.slot_id, bar_id);
    bar_id = APP_PF_BAR1;
    rc = fpga_pci_attach(aws_fpga_info.slot_id, pf_id, bar_id, 0, &aws_fpga_handle.pci_bar_handle_bar1);
    fail_on(rc, out, "Unable to attach to the AFI on slot id %d, bar_id %d", aws_fpga_info.slot_id, bar_id);
    bar_id = APP_PF_BAR4;
    rc = fpga_pci_attach(aws_fpga_info.slot_id, pf_id, bar_id, 0, &aws_fpga_handle.pci_bar_handle_pcis);
    fail_on(rc, out, "Unable to attach to the AFI on slot id %d, bar_id %d", aws_fpga_info.slot_id, bar_id);

    /* initialize the MSI interrupt fd */
    uint32_t i;
    char event_file_name[256];
    for (i=0; i<MAX_NUM_INT; i++) {
        if ((aws_fpga_info.msi_mask>>i)&0x1) {
            rc = sprintf(event_file_name, "/dev/fpga%i_event%i", aws_fpga_info.slot_id, i);
            fail_on((rc = (rc < 0)? 1:0), out, "Unable to format event file name.");
            if((aws_fpga_handle.pci_msi_fd[i] = open(event_file_name, O_RDONLY)) == -1) {
                printf("Error - invalid device %s\n", event_file_name);
                rc = 1;
                fail_on(rc, out, "Unable to open event device %s", event_file_name);
            }
        } else {
            aws_fpga_handle.pci_msi_fd[i] = -1;
        }
    }

    return (rc != 0 ? 1 : 0);
out:
    return 1;
}

static int aws_fpga_cleanup(const aws_fpga_handle_t &aws_fpga_handle)
{
    int rc = 0;;
    if (aws_fpga_handle.pci_bar_handle_ocl >= 0) {
        if (fpga_pci_detach(aws_fpga_handle.pci_bar_handle_ocl) != 0) {
            printf("Failure while detaching bar0 from the fpga.\n");
            rc = 1;
        }
    }
    if (aws_fpga_handle.pci_bar_handle_bar1 >= 0) {
        if (fpga_pci_detach(aws_fpga_handle.pci_bar_handle_bar1) != 0) {
            printf("Failure while detaching bar1 from the fpga.\n");
            rc = 1;
        }
    }
    if (aws_fpga_handle.pci_bar_handle_pcis >= 0) {
        if (fpga_pci_detach(aws_fpga_handle.pci_bar_handle_pcis) != 0) {
            printf("Failure while detaching bar4 from the fpga.\n");
            rc = 1;
        }
    }
    uint32_t i;
    for (i=0; i<MAX_NUM_INT; i++) {
        if (aws_fpga_handle.pci_msi_fd[i] != -1) {
            close(aws_fpga_handle.pci_msi_fd[i]);
        }
    }
    return rc;
}
#endif
