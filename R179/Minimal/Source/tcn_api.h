/******************************************************************************
 * COPYRIGHT    : (c) 2005 Bombardier Transportation
 ******************************************************************************
 * PRODUCT  : CSS
 *
 * ABSTRACT : CSS TCN API
 *
 ******************************************************************************
 * HISTORY  :
 *   $Log: /CSS/CSSLib/COMMON/api/tcn_api.h $
 *  
 *  12    11-08-10 10:59 Bjarne Jensen
 *  Clarified lint exception
 *  
 *  11    07-10-09 11:24 Christer Olsson
 *  Lintify
 *  
 *  10    07-09-27 9:39 Christer Olsson
 *  Removed ICT
 *  
 *  9     06-02-09 13:29 Peter Brander
 *  Headers according to coding convention
 *  
 *  8     06-01-12 13:28 Christer Olsson
 *  No more compile error 
 *  
 *  7     05-12-12 11:08 Christer Olsson
 *  Error in indirect call table
 *  
 *  6     05-11-11 17:39 Mattias Rehnman
 *  Moved typdefs and defines needed by device/application configuration
 *  from tcn_prv.h to tcn_api.h.
 *  
 *  5     05-10-27 15:50 Mattias Rehnman
 *  Corrected ICT.
 *  
 *  4     05-10-27 9:29 Mattias Rehnman
 *  Support of both indirect and direct calls.
 *  
 *  3     05-10-26 16:11 Mattias Rehnman
 *  Added indirect call table.
 *  
 *  2     05-10-07 13:27 Mattias Rehnman
 *  Removed dependency to tcn_ini.h. Defines needed in the API moved from
 *  tcn_ini.h to this file.
 *  
 *  1     05-05-02 9:18 Thomas Clevström
 *  Temporarily merged from QnD
 *  
 *  1     05-03-04 12:54 Thomas Clevström
 *  Quick-and-Dirty port of CSS for DCU2. 
 *  Mix of CSS 1.12, NG-VTCU and some CSS2.
 *  
 *****************************************************************************/
#ifndef TCN_API_H
#define TCN_API_H

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * INCLUDES
 */
/*lint -save -e659 -e620 */
/* 659: Nothing follows '}' on line terminating struct/union/class/enum definition */
/* 620: Suspicious constant (L or one?) */
/* Remove the warnings in tcn.h must be done here because a new tcn.h is 
   delivered with every new release of TCN */
#include "tcn.h"
/*lint -restore */
/*#include "css.h" */

/*****************************************************************************
 * DEFINES
 */

#define SERVICE_NAME_TCN
#define TCN_VERSION      1
#define TCN_RELEASE      6
#define TCN_UPDATE       0
#define TCN_EVOLUTION    4
    
/* These defines are needed internally and in the device configuration */
#define TCN_VB_END_OR_GW     0      /* Default */
#define TCN_ROUTER_BRIDGE    1
#define TCN_TB_END           2
#define TCN_VB_END           3

#define TCN_DISABLE_BRIDGING 0      /* Default */
#define TCN_ENABLE_BRIDGING  1

#define TCN_MAX_BUS_ID       16
    
#ifdef O_DC
/* Direct call. Actually the major part of the macros are already defined
   in tcn.h, but in this way we have better control. */
#undef apd_put_variable
#define apd_put_variable _apd_put_variable 
#undef apd_put_var48
#define apd_put_var48 _apd_put_variable 
#undef apd_get_variable
#define apd_get_variable _apd_get_variable 
#undef apd_get_var48
#define apd_get_var48 _apd_get_variable 
#undef apd_put_set
#define apd_put_set _apd_put_set 
#undef apd_put_set_fast
#define apd_put_set_fast _apd_put_set_fast 
#undef apd_get_set
#define apd_get_set _apd_get_set 
#undef apd_get_set_fast
#define apd_get_set_fast _apd_get_set_fast 
#undef apd_get_cluster
#define apd_get_cluster _apd_get_cluster 
#undef apd_put_cluster
#define apd_put_cluster _apd_put_cluster 
#undef apd_subscribe
#define apd_subscribe _apd_subscribe 
#undef apd_desubscribe
#define apd_desubscribe _apd_desubscribe 
#undef apd_pass_subscription
#define apd_pass_subscription _apd_pass_subscription 
#undef apd_force_variable
#define apd_force_variable _apd_force_variable 
#undef apd_unforce_variable
#define apd_unforce_variable _apd_unforce_variable 
#undef apd_unforce_all
#define apd_unforce_all _apd_unforce_all 
#undef ap_frc_single
#define ap_frc_single _ap_frc_single 
#undef apd_ts_create
#define apd_ts_create _lp_create 
#undef lp_create
#define lp_create _lp_create 
#undef lp_redundancy
#define lp_redundancy _lp_redundancy 
#undef apd_ts_redundancy
#define apd_ts_redundancy _lp_redundancy 
#undef lp_get_p_ts_config
#define lp_get_p_ts_config _lp_get_p_ts_config 
#undef ap_put_variable
#define ap_put_variable _ap_put_variable 
#undef apd_put_var32
#define apd_put_var32 _ap_put_variable 
#undef ap_get_variable
#define ap_get_variable _ap_get_variable 
#undef apd_get_var32
#define apd_get_var32 _ap_get_variable 
#undef apd_put_pv_x
#define apd_put_pv_x _lpl_put_pv_x 
#undef lp_put_pv_x
#define lp_put_pv_x _lpl_put_pv_x 
#undef apd_get_pv_x
#define apd_get_pv_x _lpl_get_pv_x 
#undef lp_get_pv_x
#define lp_get_pv_x _lpl_get_pv_x 
#undef apd_sizeof_pv_set
#define apd_sizeof_pv_set lpd_sizeof_pv_set
#undef apd_gen_pv_set
#define apd_gen_pv_set _lpd_gen_pv_set 
#undef apd_modify_pv_set
#define apd_modify_pv_set _lpd_mod_pv_set 
#undef lp_gen_ds_set
#define lp_gen_ds_set _lp_gen_ds_set 
#undef lp_get_data_set
#define lp_get_data_set _lp_get_data_set 
#undef lp_put_data_set
#define lp_put_data_set _lp_put_data_set 
#undef lp_cluster
#define lp_cluster _lp_cluster 
#undef lpd_get_cluster
#define lpd_get_cluster(p_clus) _lp_cluster(p_clus, LP_GET)
#undef lpd_put_cluster
#define lpd_put_cluster(p_clus) _lp_cluster(p_clus, LP_PUT)
#undef lpd_gen_clus_ts
#define lpd_gen_clus_ts _lpx_gen_clus_ts 
#undef lpd_gen_clus_ds
#define lpd_gen_clus_ds _lpx_gen_clus_ds 
#undef apd_tmo_set_state
#define apd_tmo_set_state _lp_tmo_set_state 
#undef apd_tmo_get_state
#define apd_tmo_get_state _lp_tmo_get_state 
#undef apd_tmo_counter
#define apd_tmo_counter _lp_tmo_counter 
#undef lp_open_port
#define lp_open_port _lp_open_port 
#undef lp_close_port
#define lp_close_port _lp_close_port 
#undef lp_frc_single
#define lp_frc_single _lp_frc_single 
#undef lme_get_DSW
#define lme_get_DSW _lc_m_get_device_status_word 
#undef lc_m_get_device_status_word
#define lc_m_get_device_status_word _lc_m_get_device_status_word 
#undef lc_get_device_status_word
#define lc_get_device_status_word _lc_m_get_device_status_word 
#undef lc_m_get_device_address
#define lc_m_get_device_address _lc_m_get_device_address 
#undef am_subscribe_lm
#define am_subscribe_lm _am_subscribe_lm 
#undef am_disubscribe_lm
#define am_disubscribe_lm _am_disubscribe_lm 
#undef am_announce_device
#define am_announce_device _am_announce_device 
#undef am_insert_dir_entries
#define am_insert_dir_entries _am_insert_dir_entries 
#undef am_remove_dir_entries
#define am_remove_dir_entries _am_remove_dir_entries 
#undef am_get_dir_entry
#define am_get_dir_entry _am_get_dir_entry 
#undef am_clear_dir
#define am_clear_dir _am_clear_dir 
#undef am_call_requ
#define am_call_requ(fct, r, out_a, out_s, in_a, in_s, tmo, cc, ref) \
    _am_send_requ(fct, r, out_a, out_s, in_a, in_s, tmo, cc, \
                  (AM_INVOKE_CONF)0, ref)
#undef am_call_cancel
#define am_call_cancel _am_call_cancel 
#undef am_bind_replier
#define am_bind_replier _am_bind_replier 
#undef am_unbind_replier
#define am_unbind_replier _am_unbind_replier 
#undef am_rcv_requ
#define am_rcv_requ _am_rcv_requ 
#undef am_reply_requ
#define am_reply_requ _am_reply_requ 
#undef am_rcv_cancel
#define am_rcv_cancel _am_rcv_cancel 
#undef am_st_write
#define am_st_write _am_st_write 
#undef am_st_read
#define am_st_read _am_st_read 
#undef am_set_current_tc
#define am_set_current_tc _am_set_current_tc 
#undef am_get_station_id
#define am_get_station_id _am_get_station_id 
#undef lme_am_read_messenger_status
#define lme_am_read_messenger_status _am_read_messenger_status 
#undef lc_read_mvb_status
#define lc_read_mvb_status _lc_read_mvb_status 
#undef ba_subscribe_diagnosis
#define ba_subscribe_diagnosis _bam_diag 
#undef ba_quick_scan
#define ba_quick_scan _bad_quick_scan 
#undef ba_single_poll
#define ba_single_poll _bap_single_poll 
#undef lme_am_write_messenger_control
#define lme_am_write_messenger_control _am_write_messenger_control 
#undef am_nm_config
#define am_nm_config _am_nm_config 
#undef am_get_status
#define am_get_status _am_get_status 
#undef am_get_replier_status
#define am_get_replier_status _am_get_replier_status 
#undef am_disp_show_busses
#define am_disp_show_busses _am_disp_show_busses 

#else
#warning "Compilation define 0_DC must be specified"
#endif /* O_DC */
    
/*******************************************************************************
* GLOBAL CONSTANTS                            
*/

/*****************************************************************************
 * TYPEDEFS
 */
    
/* The following typedefs are needed by device configuration */

/*
    Link Layer Common intialisation
    Reference: TCN Initialisation Description, chapter 3
*/

typedef struct STR_TCN_Q_FRAG
{
    void*                   p_tm_fragment;
    struct STR_TCN_Q_FRAG*  p_next;
    UINT32                  size;
} TCN_Q_FRAG;

typedef struct STR_TCN_LC_ENTRY
{
    void*       p_ts;
    UINT32      dvc_addr;
    UINT32      lc_cpm_cisr_clr_mvbc_int0;
    UINT32      lc_cpm_cisr_clr_mvbc_int1;
    UINT32      reserved_1;
    UINT32      reserved_2;
    TCN_Q_FRAG  tcn_q_frag_x0;
    TCN_Q_FRAG  tcn_q_frag_x1;
    TCN_Q_FRAG  tcn_q_frag_rv;
    UINT16      lc_vector_mvbc_int0;
    UINT16      lc_vector_mvbc_int1;
    UINT16      mcm;
    UINT16      q_offs;
    UINT16      m_offs;
    UINT16      t_rply;
    UINT16      channels;
    UINT16      bus_id;
} TCN_LC_ENTRY;

typedef struct STR_TCN_LC_INIT
{
    TCN_LC_ENTRY    tcn_lc_entries[TCN_MAX_BUS_ID];
    UINT32          tcn_c_lc_entries;
} TCN_LC_INIT;


/*
    Bus Administrator Intialisation
    Reference: TCN Initialisation Description, chapter 4
*/

typedef struct STR_TCN_BA_INIT
{
    UINT16            checksum;
    UINT16            version;
    UINT16            t_reply_max;
    UINT16            c_periods_in_macro;
    UINT16            poll_strategy;
    UINT16            basic_period;
    UINT16            c_macros_in_turn;
    UINT16            scan_strategy;
    UINT8             reserved[8];
    UINT16            ix_known_device;
    UINT16            reserved_2;
    UINT16            ix_cyclic_mf_list;
    UINT16            ix_bus_administrator_list;
    UINT16            ix_scan_list;
    UINT16            ix_end_of_list;
} TCN_BA_INIT;

/*
    Message Data Intialisation
    Reference: TCN Initialisation Description, chapter 5
*/

typedef struct STR_TCN_MD_INIT
{
    void *   p_fct_dir_entries;   /* function directory */
    UINT16   c_fct_dir_entries;
    UINT16   reserved1[2];
    UINT16   c_max_call;          /* max call number */
    UINT16   c_max_inst;          /* max instance number */
    UINT16   reply_timeout;       /* default reply timeout */
    UINT16   my_credit;           /* default credit */
    UINT16   reserved2;
    UINT16   operation_mode;      /* End, Router, Bridge or GW */
    UINT16   bridging_mode;       /* Enable/Disable            */
    void *   p_sta_dir_entries;   /* station directory */
    UINT16   c_sta_dir_entries;
    UINT16   reserved3;
    void *   p_group_dir_entries; /* group directory */
    UINT16   c_group_dir_entries;
    UINT16   reserved4;
    void *   p_node_dir_entries;  /* node directory */
    UINT16   c_node_dir_entries;
    UINT16   reserved5;
    void *   p_reserved;
    UINT16   c_reserved;
    UINT16   reserved6;
} TCN_MD_INIT;


/*
    Process Data Intialisation
    Reference: TCN Initialisation Description, chapter 6
*/

typedef struct STR_TCN_PD_INIT_TMO
{
    UINT16  c_tmo_list;
    UINT16  reserved;
    void*   p_tmo_list[16];
} TCN_PD_INIT_TMO;

typedef struct STR_TCN_PD_INIT_CFG
{
    struct STR_TCN_PD_INIT_CFG* p_next;
    void*                       p_lp_ts_config;
    void*                       p_lp_tmo;
    UINT16                      bus_id;
    UINT16                      hw_type;
} TCN_PD_INIT_CFG;

typedef struct STR_TCN_PD_INIT_IND
{
    void (*p_func)  (UINT16 param) ;
    UINT16          ds_name;
    UINT16          param;
} TCN_PD_INIT_IND;

typedef struct STR_TCN_PD_INIT
{
    struct STR_TCN_PD_INIT_TMO* p_pd_tmo;
    struct STR_TCN_PD_INIT_CFG* p_pd_cfg;
    struct STR_TCN_PD_INIT_IND  pd_ind[6];
} TCN_PD_INIT;

/*****************************************************************************
*   GLOBAL VARIABLES
*/

/*****************************************************************************
*   GLOBAL FUNCTION DECLARATIONS
*/

#ifdef __cplusplus
}
#endif

#endif /* TCN_API_H */
