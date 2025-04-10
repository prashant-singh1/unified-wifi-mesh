#ifndef EC_PA_CONFIGURATOR_H
#define EC_PA_CONFIGURATOR_H

#include "ec_configurator.h"

#include <map>
#include <vector>

class ec_pa_configurator_t : public ec_configurator_t {
public:
    
	/**
	 * @brief The Proxy Agent side of the EasyConnect Configurator.
	 *
	 * This constructor initializes the EasyConnect Configurator for the Proxy Agent.
	 * It handles the 802.11 frames from the Enrollee and forwards them to the Controller.
	 * It also handles the 1905 frames from the Controller and forwards them to the Enrollee.
	 *
	 * @param[in] mac_addr The MAC address of the device.
	 * @param[in] send_chirp_notification The function to send a chirp notification via 1905.
	 * @param[in] send_prox_encap_dpp_msg The function to send a proxied encapsulated DPP message via 1905.
	 * @param[in] send_action_frame The function to send an 802.11 action frame.
	 * @param[in] get_sta_info_func The function to get backhaul station information.
	 * @param[in] ieee1905_info_func The function to get 1905 information.
	 * @param[in] toggle_cce_fn The function to toggle the CCE (Centralized Control Entity).
	 *
	 * @note This constructor is part of the ec_pa_configurator_t class which extends ec_configurator_t.
	 */
	ec_pa_configurator_t(
        std::string mac_addr,
        send_chirp_func send_chirp_notification,
        send_encap_dpp_func send_prox_encap_dpp_msg,
        send_act_frame_func send_action_frame,
        get_backhaul_sta_info_func get_sta_info_func,
        get_1905_info_func ieee1905_info_func,
        toggle_cce_func toggle_cce_fn
    ) : ec_configurator_t(
            mac_addr,
            send_chirp_notification,
            send_prox_encap_dpp_msg,
            send_action_frame,
            get_sta_info_func,
            ieee1905_info_func,
            {}
        ),
        m_toggle_cce(toggle_cce_fn) { }


    
	/**
	 * @brief Handles a presence announcement 802.11 frame, performing the necessary actions and possibly passing to 1905.
	 *
	 * This function processes the given 802.11 frame and determines the appropriate actions to take. It may also forward the frame to the 1905 layer if necessary.
	 *
	 * @param[in] frame The 802.11 frame to handle.
	 * @param[in] len The length of the frame.
	 * @param[in] src_mac The source MAC address of the frame.
	 *
	 * @return true if the frame was handled successfully, false otherwise.
	 *
	 * @note This function is optional to implement because the controller+configurator does not handle 802.11 frames, but the proxy agent + configurator does.
	 */
	bool handle_presence_announcement(ec_frame_t *frame, size_t len, uint8_t src_mac[ETHER_ADDR_LEN]) override;

    
	/**
	 * @brief Handles an authentication request 802.11 frame, performing the necessary actions and possibly passing to 1905.
	 *
	 * This function processes the given 802.11 authentication frame and determines the appropriate actions to take.
	 *
	 * @param[in] frame The frame to handle.
	 * @param[in] len The length of the frame.
	 * @param[in] src_mac The source MAC address of the frame.
	 *
	 * @return true if the frame was handled successfully, false otherwise.
	 *
	 * @note This function is optional to implement because the controller+configurator does not handle 802.11,
	 *       but the proxy agent + configurator does.
	 */
	bool handle_auth_response(ec_frame_t *frame, size_t len, uint8_t src_mac[ETHER_ADDR_LEN]) override;

    
	/**
	 * @brief Handles a configuration request 802.11+GAS frame, performing the necessary actions and possibly passing to 1905.
	 *
	 * @param[in] buff The frame to handle.
	 * @param[in] len The length of the frame.
	 * @param[in] sa The 802.11 source address of the frame (from Enrollee).
	 *
	 * @return bool True if successful, false otherwise.
	 *
	 * @note Optional to implement because the controller+configurator does not handle 802.11,
	 *       but the proxy agent + configurator does.
	 */
	bool handle_cfg_request(uint8_t *buff, unsigned int len, uint8_t sa[ETH_ALEN]) override;

    
	/**
	 * @brief Handles a configuration result 802.11+GAS frame, performing the necessary actions and possibly passing to 1905.
	 *
	 * This function processes the given frame and determines the appropriate actions to take.
	 *
	 * @param[in] frame The frame to handle.
	 * @param[in] len The length of the frame.
	 * @param[in] sa The source address of the frame.
	 *
	 * @return true if the frame was handled successfully, false otherwise.
	 *
	 * @note This function is optional to implement because the controller+configurator does not handle 802.11,
	 *       but the proxy agent + configurator does.
	 */
	bool handle_cfg_result(ec_frame_t *frame, size_t len, uint8_t sa[ETH_ALEN]) override;

    
	/**
	 * @brief Handles Connection Status Result frame.
	 *
	 * This function processes the connection status result frame and determines
	 * the success or failure of the operation based on the frame's content.
	 *
	 * @param[in] frame The frame to be processed.
	 * @param[in] len The length of the frame.
	 * @param[in] sa The source address of the frame, represented as an array of
	 * bytes with a length of ETH_ALEN.
	 *
	 * @return true if the frame was processed successfully, otherwise false.
	 */
	virtual bool handle_connection_status_result(ec_frame_t *frame, size_t len, uint8_t sa[ETH_ALEN]) override;

    
	/**
	 * @brief Handle a chirp notification TLV and direct to the correct place (802.11 or 1905).
	 *
	 * This function processes the given chirp TLV and determines the appropriate handling
	 * based on its type, either for 802.11 or 1905 protocols.
	 *
	 * @param[in] chirp_tlv Pointer to the chirp TLV to parse and handle.
	 * @param[in] tlv_len The length of the chirp TLV.
	 *
	 * @return true if the chirp notification was processed successfully, false otherwise.
	 */
	bool process_chirp_notification(em_dpp_chirp_value_t* chirp_tlv, uint16_t tlv_len) override;

    
	/**
	 * @brief Handle a proxied encapsulated DPP message TLVs (including chirp value) and direct to the correct place (802.11 or 1905)
	 *
	 * This function processes the encapsulated DPP message TLVs and directs them to the appropriate protocol handler.
	 *
	 * @param[in] encap_tlv The 1905 Encap DPP TLV to parse and handle.
	 * @param[in] encap_tlv_len The length of the 1905 Encap DPP TLV.
	 * @param[in] chirp_tlv The DPP Chirp Value TLV to parse and handle. Pass NULL if not present.
	 * @param[in] chirp_tlv_len The length of the DPP Chirp Value TLV. Pass 0 if not present.
	 *
	 * @return bool True if the message was processed successfully, false otherwise.
	 */
	bool process_proxy_encap_dpp_msg(em_encap_dpp_t *encap_tlv, uint16_t encap_tlv_len, em_dpp_chirp_value_t *chirp_tlv, uint16_t chirp_tlv_len) override;

    /**
     * @brief Set the CCE IEs in the beacon and probe response frames
     * 
     * @param bool Whether to enable or disable the inclusion of CCE IEs in the beacon and probe response frames
     * @return bool true if successful, false otherwise
     * @note If the operation fails, all CCE IEs are removed before the function exits
     */
    toggle_cce_func m_toggle_cce;

private:
    // Private member variables go here
    /*
     * Map from Chirp Hash to DPP Authentication Request
     */
    std::map<std::string, std::vector<uint8_t>> m_chirp_hash_frame_map = {};
    /*
     * Vector of all cached DPP Reconfiguration Authentication Requests.
     * Hash does not matter since it is compared against the Controllers C-sign key
     */
    std::vector<std::vector<uint8_t>> m_stored_recfg_auth_frames = {};
protected:
    // Protected member variables and methods go here
};

#endif // EC_PA_CONFIGURATOR_H