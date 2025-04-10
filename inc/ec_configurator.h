#ifndef EC_CONFIGURATOR_H
#define EC_CONFIGURATOR_H

#include <functional>
#include <map>
#include <string>

#include "em_base.h"
#include "ec_crypto.h"


/**
 * @brief Sends a chirp notification
 * 
 * @param chirp_tlv The chirp TLV to send
 * @param len The length of the chirp TLV
 * @return bool true if successful, false otherwise
 */
using send_chirp_func = std::function<bool(em_dpp_chirp_value_t*, size_t)>;

/**
 * @brief Sends a proxied encapsulated DPP message
 * 
 * @param encap_dpp_tlv The 1905 Encap DPP TLV to include in the message
 * @param encap_dpp_len The length of the 1905 Encap DPP TLV
 * @param chirp_tlv The chirp value to include in the message. If NULL, the message will not include a chirp value
 * @param chirp_len The length of the chirp value
 * @return bool true if successful, false otherwise
 */
using send_encap_dpp_func = std::function<bool(em_encap_dpp_t*, size_t, em_dpp_chirp_value_t*, size_t)>;

/**
 * @brief Send an action frame. Optional to implement.
 * 
 * @param dest_mac The destination MAC address
 * @param action_frame The action frame to send
 * @param action_frame_len The length of the action frame
 * @param frequency The frequency to send the frame on (0 for current frequency)
 * @param wait The time to wait on the channel after sending the frame (0 for no wait)
 * @return true if successful, false otherwise
 */
using send_act_frame_func = std::function<bool(uint8_t*, uint8_t *, size_t, unsigned int, unsigned int)>;

/**
 * @brief Set the CCE IEs in the beacon and probe response frames
 * 
 * @param bool Whether to enable or disable the inclusion of CCE IEs in the beacon and probe response frames
 * @return bool true if successful, false otherwise
 * @note If the operation fails, all CCE IEs are removed before the function exits
 */
using toggle_cce_func = std::function<bool(bool)>;

/**
 * @brief Creates a DPP Configuration Response object for the backhaul STA interface.
 * @param conn_ctx Optional connection context (not needed for Enrollee, needed for Configurator) -- pass nullptr if not needed.
 * @return cJSON * on success, nullptr otherwise
 */
using get_backhaul_sta_info_func = std::function<cJSON*(ec_connection_context_t *)>;

/**
 * @brief Creates a DPP Configuration Response object for the 1905.1 interface.
 * @return cJSON * on success, nullptr otherwise.
 */
using get_1905_info_func = std::function<cJSON*(ec_connection_context_t *)>;

/**
 * @brief Used to determine if an additional AP can be on-boarded or not.
 * @return True if additional APs can be on-boraded into the mesh, false otherwise.
 */
using can_onboard_additional_aps_func = std::function<bool(void)>;

class ec_configurator_t {
public:
    
	/**
	 * @brief Construct an EC configurator
	 *
	 * This constructor initializes the EC configurator with the specified functions
	 * for sending notifications and messages.
	 *
	 * @param[in] mac_addr The MAC address of the device
	 * @param[in] send_chirp_notification The function to send a chirp notification
	 * @param[in] send_prox_encap_dpp_msg The function to send a proxied encapsulated DPP message
	 * @param[in] send_action_frame The function to send an action frame
	 * @param[in] backhaul_sta_info_func The function to get backhaul station information
	 * @param[in] ieee1905_info_func The function to get IEEE 1905 information
	 * @param[in] can_onboard_func The function to check if additional APs can be onboarded
	 */
	ec_configurator_t(std::string mac_addr, send_chirp_func send_chirp_notification, send_encap_dpp_func send_prox_encap_dpp_msg, 
                        send_act_frame_func send_action_frame, get_backhaul_sta_info_func backhaul_sta_info_func, get_1905_info_func ieee1905_info_func,
                        can_onboard_additional_aps_func can_onboard_func);
    
	/**!
	 * @brief Destructor for ec_configurator_t class.
	 *
	 * This destructor cleans up any resources allocated by the ec_configurator_t instance.
	 *
	 * @note Ensure that all necessary cleanup operations are performed before the destructor is called.
	 */
	virtual ~ec_configurator_t(); // Destructor

    
	/**
	 * @brief Start the EC configurator onboarding process for an enrollee.
	 *
	 * This function initiates the onboarding process using the provided bootstrapping data.
	 *
	 * @param[in] bootstrapping_data The data to use for onboarding (Parsed DPP URI Data).
	 *
	 * @return bool True if the onboarding process is successful, false otherwise.
	 */
	bool onboard_enrollee(ec_data_t* bootstrapping_data);

    
	/**
	 * @brief Handles a presence announcement 802.11 frame, performing the necessary actions.
	 *
	 * @param[in] frame The frame to handle.
	 * @param[in] len The length of the frame.
	 * @param[in] src_mac The source MAC address.
	 *
	 * @return bool True if successful, false otherwise.
	 *
	 * @note Optional to implement because the controller+configurator does not handle 802.11,
	 *       but the proxy agent + configurator does.
	 */
	virtual bool handle_presence_announcement(ec_frame_t *frame, size_t len, uint8_t src_mac[ETHER_ADDR_LEN]) {
        return 0; // Optional to implement
    }

    
	/**
	 * @brief Handles an authentication request 802.11 frame, performing the necessary actions.
	 *
	 * This function processes the given 802.11 authentication frame and executes the required
	 * operations based on the frame's content.
	 *
	 * @param[in] frame The 802.11 frame to handle.
	 * @param[in] len The length of the frame.
	 * @param[in] src_mac The source MAC address of the frame.
	 *
	 * @return true if the frame was handled successfully, false otherwise.
	 *
	 * @note This function is optional to implement because the controller+configurator does not
	 * handle 802.11 frames, but the proxy agent + configurator does.
	 */
	virtual bool handle_auth_response(ec_frame_t *frame, size_t len, uint8_t src_mac[ETHER_ADDR_LEN]) {
        return true; // Optional to implement
    }

    
	/**
	 * @brief Handles a configuration request 802.11+GAS frame, performing the necessary actions.
	 *
	 * This function processes the incoming 802.11+GAS frame and executes the required operations
	 * based on the frame's content and source address.
	 *
	 * @param[in] buff The frame to handle.
	 * @param[in] len The length of the frame.
	 * @param[in] sa The 802.11 source address of the frame (from Enrollee).
	 *
	 * @return bool True if the operation is successful, false otherwise.
	 *
	 * @note This function is optional to implement because the controller+configurator does not handle 802.11,
	 *       but the proxy agent + configurator does.
	 */
	virtual bool handle_cfg_request(uint8_t *buff, unsigned int len, uint8_t sa[ETH_ALEN]) {
        return true; // Optional to implement
    }

    
	/**
	 * @brief Handles a configuration result 802.11+GAS frame, performing the necessary actions.
	 *
	 * This function processes a given 802.11+GAS frame and executes the required operations based on the frame's content.
	 *
	 * @param[in] frame The frame to handle.
	 * @param[in] len The length of the frame.
	 * @param[in] sa The source address of the frame.
	 *
	 * @return bool True if the operation is successful, false otherwise.
	 *
	 * @note This function is optional to implement because the controller+configurator does not handle 802.11,
	 *       but the proxy agent + configurator does.
	 */
	virtual bool handle_cfg_result(ec_frame_t *frame, size_t len, uint8_t sa[ETH_ALEN]) {
        return true; // Optional to implement
    }

    
	/**
	 * @brief Handles Connection Status Result frame.
	 *
	 * This function processes the connection status result frame and determines
	 * the success or failure of the operation based on the frame content.
	 *
	 * @param[in] frame Pointer to the frame data structure.
	 * @param[in] len Length of the frame.
	 * @param[in] sa Source address of the frame, represented as an array of
	 *               unsigned 8-bit integers.
	 *
	 * @return true if the operation is successful, otherwise false.
	 */
	virtual bool handle_connection_status_result(ec_frame_t *frame, size_t len, uint8_t sa[ETH_ALEN]) {
        return true;
    }

    
	/**
	 * @brief Handle a chirp notification message TLV and direct it to the correct place (802.11 or 1905).
	 *
	 * This function processes the chirp notification by parsing the provided TLV and determining
	 * the appropriate handling mechanism based on the TLV content.
	 *
	 * @param[in] chirp_tlv Pointer to the chirp TLV to parse and handle.
	 * @param[in] tlv_len The length of the chirp TLV.
	 *
	 * @return true if the chirp notification was processed successfully, false otherwise.
	 */
	virtual bool process_chirp_notification(em_dpp_chirp_value_t* chirp_tlv, uint16_t tlv_len) = 0;

    
	/**
	 * @brief Handle a proxied encapsulated DPP message TLVs (including chirp value) and direct to the correct place (802.11 or 1905)
	 *
	 * This function processes the encapsulated DPP message TLVs and directs them to the appropriate protocol handler.
	 *
	 * @param[in] encap_tlv The 1905 Encap DPP TLV to parse and handle.
	 * @param[in] encap_tlv_len The length of the 1905 Encap DPP TLV.
	 * @param[in] chirp_tlv The DPP Chirp Value TLV to parse and handle (NULL if not present).
	 * @param[in] chirp_tlv_len The length of the DPP Chirp Value TLV (0 if not present).
	 *
	 * @return bool True if the message was processed successfully, false otherwise.
	 */
	virtual bool  process_proxy_encap_dpp_msg(em_encap_dpp_t *encap_tlv, uint16_t encap_tlv_len, em_dpp_chirp_value_t *chirp_tlv, uint16_t chirp_tlv_len) = 0;

    
	/**
	 * @brief Handle a proxied encapsulated DPP Configuration Request frame.
	 *
	 * This function processes a DPP Configuration Request frame received from an Enrollee.
	 *
	 * @param[in] encap_frame The DPP Configuration Request frame from an Enrollee.
	 * @param[in] encap_frame_len The length of the DPP Configuration Request frame.
	 * @param[in] dest_mac The source MAC of this DPP Configuration Request frame (Enrollee).
	 *
	 * @return true on success, otherwise false.
	 *
	 * @note This function is overridden by subclass.
	 */
	virtual bool handle_proxied_dpp_configuration_request(uint8_t *encap_frame, uint16_t encap_frame_len, uint8_t dest_mac[ETH_ALEN]) {
        return true;
    }

    
	/**
	 * @brief Handle a proxied encapsulated DPP Configuration Result frame.
	 *
	 * This function processes a DPP Configuration Result frame that has been
	 * encapsulated and proxied. It extracts necessary information from the frame
	 * and performs required operations based on the configuration result.
	 *
	 * @param[in] encap_frame Pointer to the DPP Configuration Result frame.
	 * @param[in] encap_frame_len Length of the encapsulated frame.
	 * @param[in] dest_mac Source MAC address of the DPP Configuration Result frame (Enrollee).
	 *
	 * @return true if the frame is handled successfully, otherwise false.
	 */
	virtual bool handle_proxied_config_result_frame(uint8_t *encap_frame, uint16_t encap_frame_len, uint8_t dest_mac[ETH_ALEN]) {
        return true;
    }

    
	/**
	 * @brief Handle a proxied encapsulated DPP Connection Status Result frame.
	 *
	 * This function processes a DPP Connection Status Result frame that has been
	 * encapsulated and proxied. It extracts necessary information from the frame
	 * and performs required operations based on the connection status.
	 *
	 * @param[in] encap_frame Pointer to the DPP Connection Status Result frame.
	 * @param[in] encap_frame_len Length of the encapsulated frame.
	 * @param[in] dest_mac Source MAC address of the DPP Connection Status Result frame (Enrollee).
	 *
	 * @return true if the frame was handled successfully, otherwise false.
	 */
	virtual bool handle_proxied_conn_status_result_frame(uint8_t *encap_frame, uint16_t encap_frame_len, uint8_t dest_mac[ETH_ALEN]) {
        return true;
    }

    
	/**!
	 * @brief Tears down the connection associated with the given MAC address.
	 *
	 * This function searches for the connection using the provided MAC address and, if found,
	 * frees the associated connection and ephemeral contexts.
	 *
	 * @param[in] mac The MAC address of the connection to be torn down.
	 *
	 * @note If the connection is not found, the function returns immediately without performing any action.
	 */
	inline void teardown_connection(const std::string& mac) {
        auto conn = m_connections.find(mac);
        if (conn == m_connections.end()) return;
        auto &c_ctx = conn->second;
        ec_crypto::free_connection_ctx(&c_ctx);
        ec_crypto::free_ephemeral_context(&c_ctx.eph_ctx, c_ctx.nonce_len, c_ctx.digest_len);
    }

    
	/**!
	 * @brief Retrieves the MAC address.
	 *
	 * @returns The MAC address as a string.
	 */
	inline std::string get_mac_addr() { return m_mac_addr; };

    // Disable copy construction and assignment
    // Requires use of references or pointers when working with instances of this class
    
	/**!
	 * @brief Deleted copy constructor for ec_configurator_t.
	 *
	 * This constructor is deleted to prevent copying of ec_configurator_t instances.
	 *
	 * @note Copying of ec_configurator_t is not allowed to ensure unique configuration instances.
	 */
	ec_configurator_t(const ec_configurator_t&) = delete;
    ec_configurator_t& operator=(const ec_configurator_t&) = delete;

protected:

    std::string m_mac_addr;

    send_chirp_func m_send_chirp_notification;

    send_encap_dpp_func m_send_prox_encap_dpp_msg;

    send_act_frame_func m_send_action_frame;

    get_backhaul_sta_info_func m_get_backhaul_sta_info;

    get_1905_info_func m_get_1905_info;

    can_onboard_additional_aps_func m_can_onboard_additional_aps;

    // The connections to the Enrollees/Agents
    std::map<std::string, ec_connection_context_t> m_connections = {};

    
	/**!
	 * @brief Retrieves the connection context for a given MAC address.
	 *
	 * This function searches for the connection context associated with the specified MAC address.
	 * If the MAC address is not found, it returns NULL.
	 *
	 * @param[in] mac The MAC address for which the connection context is requested.
	 *
	 * @returns A pointer to the connection context associated with the given MAC address.
	 * @retval NULL if the MAC address is not found in the connections.
	 *
	 * @note Ensure that the MAC address provided is valid and exists in the connections map.
	 */
	inline ec_connection_context_t* get_conn_ctx(const std::string& mac) {
        if (m_connections.find(mac) == m_connections.end()) {
            return NULL;
        }
        return &m_connections[mac];    
    }

    
	/**!
	 * @brief Retrieves the ephemeral context for a given MAC address.
	 *
	 * This function attempts to find the connection context associated with the provided MAC address
	 * and returns a pointer to its ephemeral context if found.
	 *
	 * @param[in] mac The MAC address of the enrollee for which the ephemeral context is requested.
	 *
	 * @returns A pointer to the ephemeral context associated with the given MAC address.
	 * @retval NULL If the connection context for the given MAC address is not found.
	 *
	 * @note Ensure that the MAC address provided is valid and corresponds to an existing connection context.
	 */
	inline ec_ephemeral_context_t* get_eph_ctx(const std::string& mac) {
        auto conn_ctx = get_conn_ctx(mac);
        if (!conn_ctx) {
            printf("%s:%d: Connection context not found for enrollee MAC %s\n", __func__, __LINE__, mac.c_str());
            return NULL;  // Return reference to static empty context
        }
        return &conn_ctx->eph_ctx;
    }

    
	/**!
	 * @brief Retrieves the boot data for a given MAC address.
	 *
	 * This function searches for the connection context associated with the provided MAC address
	 * and returns a pointer to the boot data if found.
	 *
	 * @param[in] mac The MAC address of the enrollee whose boot data is to be retrieved.
	 *
	 * @returns A pointer to the boot data associated with the given MAC address.
	 * @retval NULL if the connection context for the given MAC address is not found.
	 *
	 * @note Ensure that the MAC address provided is valid and corresponds to an existing connection.
	 */
	inline ec_data_t* get_boot_data(const std::string& mac) {
        auto conn = m_connections.find(mac);
        if (conn == m_connections.end()) {
            printf("%s:%d: Connection context not found for enrollee MAC %s\n", __func__, __LINE__, mac.c_str());
            return NULL;  // Return reference to static empty context
        }
        return &conn->second.boot_data;
    }

    
	/**!
	 * @brief Clears the ephemeral context for a given MAC address.
	 *
	 * This function searches for a connection associated with the provided MAC address
	 * and clears its ephemeral context if found.
	 *
	 * @param[in] mac The MAC address of the connection whose ephemeral context is to be cleared.
	 *
	 * @note If the MAC address is not found in the connections, the function returns without action.
	 */
	inline void clear_conn_eph_ctx(const std::string& mac) {
        auto conn = m_connections.find(mac);
        if (conn == m_connections.end()) return;
        auto &c_ctx = conn->second;
        ec_crypto::free_ephemeral_context(&c_ctx.eph_ctx, c_ctx.nonce_len, c_ctx.digest_len);
    }

};

#endif // EC_CONFIGURATOR_H