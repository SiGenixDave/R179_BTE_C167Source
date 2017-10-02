typedef unsigned char       UINT8;
typedef unsigned short int  UINT16;
typedef unsigned int        UINT32;

typedef char                INT8;
typedef short int           INT16;
typedef int                 INT32;

typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned int        DWORD;

#define tcn_hamster_uncond(type, action, expl)                      \
            {             { printstr ((char*) expl); } }

#define AS_MAX_SIGNAL_LENGTH 26
#define TCN_OK               0
#define TCN_ERROR            1
#define RTSS_MODE_RUN        3
#define OK					 0

#define TCN_LC_CH_A          1
#define TCN_LC_CH_B          0
#define TCN_LC_CH_BOTH       2
#define TCN_LC_CH_FOPBI      3   /* Fiber Optical PBI */
                                 /* belongs not to LC */

#define TCN_LC_0_TO_256K     0  /* MF-Table/Queue Offset Values */
#define TCN_LC_256_TO_512K   1
#define TCN_LC_512_TO_768K   2
#define TCN_LC_768K_TO_1M    3

#define TCN_LC_TREPLY_21US   0  /* Reply Timeout Coefficients */
#define TCN_LC_TREPLY_43US   1
#define TCN_LC_TREPLY_64US   2
#define TCN_LC_TREPLY_85US   3

#define TCN_LC_MCM_16K       0  /* Memory Config. Modes */
#define TCN_LC_MCM_32K       1
#define TCN_LC_MCM_64K       2
#define TCN_LC_MCM_256K      3
#define TCN_LC_MCM_1M        4

#if 0 /*QQQ_DCU2*/
#define TCN_LC_FOPBI_ADDR    0x02800000L /* Fiber optical PBI */
#define TCN_LC_SRAB_ADDR     0x02980000L /* Service Redundant AB */
#endif /*QQQ_DCU2*/


/* constants for VCUC-MVBC default configuration */
#define TS_ADDR TRAFFIC_MEM_ADRS  

#define TCN_DEF_Q_OFFSET                TCN_LC_0_TO_256K
#define TCN_DEF_M_OFFSET                TCN_LC_0_TO_256K
#define TCN_DEF_T_REPLY                 TCN_LC_TREPLY_43US
#define TCN_DEF_MCM                     TCN_LC_MCM_256K
#define TCN_DEF_BUS_ID                  0x00
#define TCN_DEF_CHANNELS                TCN_LC_CH_A
#define TCN_DEF_DVC_ADDR                0x00
#define TCN_DEF_P_TS                    ((void *) TS_ADDR)

#define TCN_DEF_Q_FRAG_X0_P_TM          ((void *) (TS_ADDR+0x3C000L))
#define TCN_DEF_Q_FRAG_X0_P_NEXT        NULL
#define TCN_DEF_Q_FRAG_X0_SIZE          112
#define TCN_DEF_Q_FRAG_X0               {TCN_DEF_Q_FRAG_X0_P_TM, \
                                         TCN_DEF_Q_FRAG_X0_P_NEXT,\
                                         TCN_DEF_Q_FRAG_X0_SIZE}

#define TCN_DEF_Q_FRAG_X1_P_TM          ((void *) (TS_ADDR+0x3D000L))
#define TCN_DEF_Q_FRAG_X1_P_NEXT        NULL
#define TCN_DEF_Q_FRAG_X1_SIZE          112
#define TCN_DEF_Q_FRAG_X1               {TCN_DEF_Q_FRAG_X1_P_TM,  \
                                         TCN_DEF_Q_FRAG_X1_P_NEXT,\
                                         TCN_DEF_Q_FRAG_X1_SIZE}

#define TCN_DEF_Q_FRAG_RV_P_TM          ((void *) (TS_ADDR+0x3E000L))
#define TCN_DEF_Q_FRAG_RV_P_NEXT        NULL
#define TCN_DEF_Q_FRAG_RV_SIZE          224
#define TCN_DEF_Q_FRAG_RV               {TCN_DEF_Q_FRAG_RV_P_TM,  \
                                         TCN_DEF_Q_FRAG_RV_P_NEXT,\
                                         TCN_DEF_Q_FRAG_RV_SIZE}


/* constants for VCUC-IP-MVBC default configuration */
#define TS_IP_ADDR MVB2_TRAFFIC_MEM_ADRS       /* TEST FOR IP-Module MVB    */

#define TCN_DEF_IP_Q_OFFSET                TCN_LC_0_TO_256K
#define TCN_DEF_IP_M_OFFSET                TCN_LC_0_TO_256K
#define TCN_DEF_IP_T_REPLY                 TCN_LC_TREPLY_43US
#define TCN_DEF_IP_MCM                     TCN_LC_MCM_256K
#define TCN_DEF_IP_BUS_ID                  0x02
#define TCN_DEF_IP_CHANNELS                TCN_LC_CH_A
#define TCN_DEF_IP_DVC_ADDR                0x00
#define TCN_DEF_IP_P_TS                    ((void *) TS_IP_ADDR)

#define TCN_DEF_IP_Q_FRAG_X0_P_TM          ((void *) (TS_IP_ADDR+0x3C000L))
#define TCN_DEF_IP_Q_FRAG_X0_P_NEXT        NULL
#define TCN_DEF_IP_Q_FRAG_X0_SIZE          112
#define TCN_DEF_IP_Q_FRAG_X0               {TCN_DEF_IP_Q_FRAG_X0_P_TM, \
                                         TCN_DEF_IP_Q_FRAG_X0_P_NEXT,\
                                         TCN_DEF_IP_Q_FRAG_X0_SIZE}

#define TCN_DEF_IP_Q_FRAG_X1_P_TM          ((void *) (TS_IP_ADDR+0x3D000L))
#define TCN_DEF_IP_Q_FRAG_X1_P_NEXT        NULL
#define TCN_DEF_IP_Q_FRAG_X1_SIZE          112
#define TCN_DEF_IP_Q_FRAG_X1               {TCN_DEF_IP_Q_FRAG_X1_P_TM,  \
                                         TCN_DEF_IP_Q_FRAG_X1_P_NEXT,\
                                         TCN_DEF_IP_Q_FRAG_X1_SIZE}

#define TCN_DEF_IP_Q_FRAG_RV_P_TM          ((void *) (TS_IP_ADDR+0x3E000L))
#define TCN_DEF_IP_Q_FRAG_RV_P_NEXT        NULL
#define TCN_DEF_IP_Q_FRAG_RV_SIZE          224
#define TCN_DEF_IP_Q_FRAG_RV               {TCN_DEF_IP_Q_FRAG_RV_P_TM,  \
                                         TCN_DEF_IP_Q_FRAG_RV_P_NEXT,\
                                         TCN_DEF_IP_Q_FRAG_RV_SIZE}

/* interrupt constants for the DCU2 */

#define TCN_DEF_CPM_CISR_CLSR_MVBC_INT0         0xFFFFFFFF /* not used */
#define TCN_DEF_CPM_CISR_CLSR_MVBC_INT1         0xFFFFFFFF /* not used */

/* We now have a demux of all muxed IRQs using hwi_irq_connect()
 * All these "virtual" INUMS are found in hwi_prv.h
 */
#define TCN_DEF_VECTOR_MVBC1_INT0        HWI_IRQ_MVB_0
#define TCN_DEF_VECTOR_MVBC1_INT1        HWI_IRQ_MVB_1

#define TCN_DEF_VECTOR_MVBC2_INT0        HWI_IRQ_IPAC_A0
#define TCN_DEF_VECTOR_MVBC2_INT1        HWI_IRQ_IPAC_A1

typedef struct STR_VERSION_INFO
{
    BYTE    ver;
    BYTE    rel;
    BYTE    upd;
    BYTE    evo;
} VERSION_INFO;

typedef struct STR_VM_HEADER
{
    DWORD               checksum;
    DWORD               crc32;
    DWORD               size;
    DWORD               p_code_start;
    CHAR                magic_word[8];
    VERSION_INFO        version;
    CHAR                name[16];
    DWORD               date;
    DWORD               type;
    CHAR                filename[32];
    CHAR                product_name[16];
    CHAR                comment[120];
    VERSION_INFO        tool_version;
    CHAR                tool_name[16];
    BYTE                padding[16];
} VM_HEADER;

typedef VERSION_INFO AS_VERSION;

typedef VM_HEADER    AS_DLU_HEADER;

typedef struct STR_AS_CHECK
{
    DWORD               mvb_time_stamp;
    AS_VERSION          mvb_version;
    char                hw_config[13];
    char                device_name[9];
    BYTE                padding[2];
} AS_CHECK;

/* 
 *     Forward declarations 
 */
struct STR_AS_DEV_ENTRY;
struct STR_AS_APP_ENTRY;

/*
 *   Device Configuration Structures
 */

typedef struct STR_AS_APPLICATION
{
    struct STR_AS_APP_ENTRY* p_app;
    char                exp_app_name[16];
    AS_VERSION          exp_app_version;
    int                    deferred_start;
} AS_APPLICATION;

typedef struct STR_AS_TS_INIT
{
    DWORD               p_ts_cfg;
    DWORD               padding;
} AS_TS_INIT;

typedef struct STR_AS_MD_INIT
{
    DWORD               p_dir_entries;
    WORD                nr_dir_entries;
    DWORD               reserved;
    WORD                max_call_number;
    WORD                max_inst_number;
    WORD                default_reply_tmo;
    WORD                my_credit;
    WORD                reserved2;
    WORD                operation_mode;
    WORD                bridging_mode;
    DWORD               p_station_dir_entries;
    WORD                nr_station_dir_entries;
    WORD                size_station_dir_entries;
    DWORD               p_group_dir_entries;
    WORD                nr_group_dir_entries;
    WORD                size_group_dir_entries;
    DWORD               node_dir_entries;
    WORD                nr_node_dir_entries;
    WORD                size_node_dir_entries;
    DWORD               p_reserved;
    WORD                nr_reserved;
    WORD                size_reserved;
} AS_MD_INIT;

typedef struct STR_AS_BA_INIT
{
    DWORD               p_config_list;
    AS_VERSION          config_version;
    DWORD               config_date;
    CHAR                config_name[12];
    BYTE                bus_id;               /*AS-1110*/
    BYTE                padding[3];           /*AS-1110*/
} AS_BA_INIT;

typedef struct STR_AS_MVB_HW_CONFIG
{
    BYTE                mvbc_idc;
    BYTE                mvb_2_hw_type;
    BYTE                mvb_2_bus_id;
    BYTE                mvb_2_idc;
    BYTE                padding[8];
} AS_MVB_HW_CONFIG;

typedef struct STR_AS_DEVICE_INIT
{
    WORD                base_cycle_time;
    WORD                reset_counter;
    BYTE                wdog_flag;
    BYTE                checksum_flag;
    BYTE                ram_test_flag;
    BYTE                cpu_load_meas;
    char*               io_device;
    char*               mon_device;
    DWORD*              appl_event_address;
    DWORD               appl_event_size;
    BYTE                self_test_action_init;
    BYTE                self_test_action_rt;
    WORD                reserved;
    AS_MVB_HW_CONFIG    mvb_hw_config;
    DWORD               memory_log_size;
    BYTE                memory_log_flag;
    BYTE                memory_log_behaviour;
    BYTE                memory_log_output;
    BYTE                wdog_action;
    BYTE                mon_ca_md_fct;        /*AS-1105*/
    BYTE                padding[3];
} AS_DEVICE_INIT;

typedef struct STR_AS_MEM_CHECK
{
    BYTE*               p_check_mem;
    DWORD               size;
    BYTE                fill_flag;
    BYTE                fill_pattern;
    BYTE                mem_test_flag;
    BYTE                padding;
} AS_MEM_CHECK;

typedef struct STR_AS_DM_INIT
{
    DWORD*              p_mem_start;
    DWORD               size;
    BYTE                type;
    BYTE                identity;
    WORD                padding;
} AS_DM_INIT;

typedef struct STR_AS_DR_INIT
{
    UINT8               dummy_1;
    UINT8               dummy_2;
    UINT16              padding;
} AS_DR_INIT;

typedef struct STR_AS_DAYLIGHT
{
    UINT16              DST_mounth;
    UINT16              DST_day;
    INT16               DST_week;
    UINT16              DST_time_hh;
    UINT16              DST_time_mm;
    INT16               DST_diff;
    UINT16              ST_mounth;
    UINT16              ST_day;
    INT16               ST_week;
    UINT16              ST_time_hh;
    UINT16              ST_time_mm;
    INT16               ST_diff;
} AS_DAYLIGHT;

typedef struct STR_AS_TIME_SYNC_INIT
{
    UINT16              type;
    UINT16              traffic_store_id;
    UINT16              port;
    UINT16              cycle_time;
    UINT16              time_source;
    INT16               time_zone;
    UINT16              drift_zone;
    AS_DAYLIGHT         daylight_alg;
    UINT16              padding;
} AS_TIME_SYNC_INIT;

typedef struct STR_AS_DEV_INFO
{
    WORD                dev_address;
    WORD                index_TS;
    WORD                index_MD;
    WORD                index_BA;           /*AS-1110*/
} AS_DEV_INFO;

typedef struct STR_AS_DEV_PTR_TAB
{
    AS_APPLICATION*     p_application;
    WORD                nr_application;
    WORD                size_application;
    AS_TS_INIT*         p_ts_init;
    WORD                nr_ts_init;
    WORD                size_ts_init;
    AS_MD_INIT*         p_md_init;
    WORD                nr_md_init;
    WORD                size_md_init;
    AS_BA_INIT*         p_ba_init;
    WORD                nr_ba_init;
    WORD                size_ba_init;
    AS_DEVICE_INIT*     p_device_init;
    WORD                nr_device_init;
    WORD                size_device_init;
    AS_MEM_CHECK*       p_mem_check;
    WORD                nr_mem_check;
    WORD                size_mem_check;
    AS_DM_INIT*         p_dm_init;
    WORD                nr_dm_init;
    WORD                size_dm_init;
    AS_DR_INIT*         p_spare_8;
    WORD                nr_spare_8;
    WORD                size_spare_8;
    AS_TIME_SYNC_INIT*  p_time_sync_init;
    WORD                nr_time_sync_init;
    WORD                size_time_sync_init;
    AS_DEV_INFO*        p_dev_info;
    WORD                nr_dev_info;
    WORD                size_dev_info;
} AS_DEV_PTR_TAB;


/*
 *   Application Structures
 */

typedef struct STR_AS_DEPENDENCIES
{
    struct STR_AS_APP_ENTRY*    p_entry;
    char                        exp_name[16];
    AS_VERSION                  exp_version;
} AS_DEPENDENCIES;

typedef void AS_TASK_FUNCTION (DWORD argc, CHAR* argv[] );

typedef struct STR_AS_TASK_CONFIG
{
    AS_TASK_FUNCTION*   p_task_init;
    AS_TASK_FUNCTION*   p_task_body;
    CHAR**              appl_argv;
    DWORD               appl_argc;
    WORD                stack_size;
    WORD                int_nr;
    char                task_name[16];
    WORD                task_cycle;
    WORD                task_delay;
    WORD                wdog_delay;
    BYTE                task_type;
    BYTE                task_priority;
    BYTE                init_addr_type;
    BYTE                body_addr_type;
    WORD                expected_dev_addr;
    BYTE                padding[12];
} AS_TASK_CONFIG;

typedef struct  STR_AS_DYN_MEMORY
{
    char*               p_free_mem;
    DWORD               size;
} AS_DYN_MEMORY;

typedef struct STR_AS_COPY_LIST
{
    BYTE*               p_target;
    BYTE*               p_source;
    DWORD               size;
} AS_COPY_LIST;

typedef struct STR_AS_INIT_DATA
{
    BYTE*               p_target;
    DWORD               size;
    WORD                value;
    WORD                reserved;
} AS_INIT_DATA;

typedef struct STR_AS_APPL_DATA
{
    DWORD               p_appl_data_table;
    WORD                nr_appl_data_table;
    WORD                size_appl_data_table;
} AS_APPL_DATA;

typedef struct STR_AS_DATA_DICTIONARY
{
    BYTE                signal_name[AS_MAX_SIGNAL_LENGTH];
    DWORD*              signal_addr;
    BYTE                signal_type;
} AS_DATA_DICTIONARY;

typedef struct STR_AS_APP_PTR_TAB
{
    AS_DEPENDENCIES*    p_dependencies;
    WORD                nr_dependencies;
    WORD                size_dependencies;
    AS_TASK_CONFIG*     p_task_config;
    WORD                nr_task_config;
    WORD                size_task_config;
    AS_DYN_MEMORY*      p_dyn_memory;
    WORD                nr_dyn_memory;
    WORD                size_dyn_memory;
    AS_COPY_LIST*       p_copy_list;
    WORD                nr_copy_list;
    WORD                size_copy_list;
    AS_INIT_DATA*       p_init_data;
    WORD                nr_init_data;
    WORD                size_init_data;
    AS_APPL_DATA*       p_appl_data;
    WORD                nr_appl_data;
    WORD                size_appl_data;
    AS_DATA_DICTIONARY* p_data_dictionary;
    WORD                nr_data_dictionary;
    WORD                size_data_dictionary;
} AS_APP_PTR_TAB;

typedef struct STR_AS_DEV_ENTRY
{
    AS_DLU_HEADER       header;
    AS_CHECK            check;
    AS_DEV_PTR_TAB*     p_pointer_table;
    WORD                nr_pointer_table;
    WORD                size_pointer_table;
} AS_DEV_ENTRY;

