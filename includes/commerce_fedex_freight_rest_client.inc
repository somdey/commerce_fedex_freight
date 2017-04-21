<?php

/**
 * @file
 * Handles the REST request/response to FedEx Freight Web Services servers.
 */

/**
 * Function to create FedEx Freight rate request array.
 *
 * @param object $order
 *   The order object.
 *
 * @return array
 *   The array that is created for getting rates from FedEx Freight.
 */
function commerce_fedex_freight_create_rate_request($order) {
  $order_wrapper = entity_metadata_wrapper('commerce_order', $order);
  $weight = commerce_physical_order_weight($order, 'lb');

  // Load the number of packages and their physical attributes.
  $package_line_items = _commerce_fedex_get_package_items($order, $order_wrapper);

  $commerce_customer_shipping = commerce_customer_profile_load($order->commerce_customer_shipping[LANGUAGE_NONE][0]['profile_id']);
  $as_cncntry = $commerce_customer_shipping->commerce_customer_address[LANGUAGE_NONE][0]['country'];
  $as_cnzip = $commerce_customer_shipping->commerce_customer_address[LANGUAGE_NONE][0]['postal_code'];
  $as_cncity = $commerce_customer_shipping->commerce_customer_address[LANGUAGE_NONE][0]['locality'];
  $as_cnstate = $commerce_customer_shipping->commerce_customer_address[LANGUAGE_NONE][0]['administrative_area'];
  $as_cnzip = preg_replace('/\s+/', '', $as_cnzip);
  // Make sure that there are packages to be sent in the request.
  $serivce_url = "http://fedexfreight.fedex.com/XMLLTLRating.jsp";
  if (!empty($package_line_items)) {
    $data = array();
    $data['regKey'] = variable_get('commerce_fedex_freight_account_number').'|'.variable_get('commerce_fedex_freight_reg_key');
    $data['as_opco'] = 'Economy';
    $data['as_iamthe'] = 'shipper';
    $data['as_shipterms'] = 'prepaid';
    $data['as_shzip'] = variable_get('commerce_fedex_freight_postal_code');
    $data['as_shcntry'] = variable_get('commerce_fedex_freight_country_code');
    $data['as_shcity'] = variable_get('commerce_fedex_freight_city');
    $data['as_shstate'] = variable_get('commerce_fedex_freight_state');
    $data['as_cnzip'] = $as_cnzip;
    $data['as_cncntry'] = $as_cncntry;
    $data['as_cncity'] = $as_cncity;
    $data['as_cnstate'] = $as_cnstate;
    $data['as_class1'] = '070';
    $data['as_weight1'] = $weight['weight'];
    $uri = url($serivce_url, array('query' => $data, 'external' => TRUE));
    $options['method'] = 'GET';
    $response = drupal_http_request($uri, $options);
    $return_obj = simplexml_load_string($response->data);
    //print_r($return_obj); exit();
    // Log the API response if specified.
    $logging_config = variable_get('commerce_fedex_freight_log');
    if (!empty($logging_config['response']) && $logging_config['response'] !== 0) {
      watchdog('fedex_freight', 'API request and response:<pre>@response</pre>', array('@response' => var_export($response, TRUE)));
    }
    // Return the $response.
    return $return_obj;
  }
  else {
    // If there are no shippable packages in the order.
    return FALSE;
  }
}


/**
 * Internal function to determine freight rate class.
 *
 * @param object $order
 *   The commerce order object for the order that we're requesting rates for.
 *
 * @return string
 *   The rate class string fedex uses for freight shipments
 */
function _commerce_fedex_freight_get_rate_class($order, $order_wrapper) {
  // Using this http://www.freightcenter.com/freight%20tools/noi%20calc.htm
  $rate_classes = array(
    '0' => 500,
    '0.5' => 400,
    '1' => 300,
    '2' => 250,
    '3' => 200,
    '4' => 175,
    '5' => 150,
    '6' => 125,
    '8' => 100,
    '10' => 92.5,
    '12' => 85,
    '15' => 70,
    '20' => 60,
    '30' => 55,
    '35' => 50,
  );

  $weight = commerce_physical_order_weight($order, 'lb');
  $volume = commerce_physical_order_volume($order, 'in');
  $pcf = $weight['weight'] / $volume['volume'];

  foreach ($rate_classes as $density => $class) {
    if ($pcf > (float) $density) {
      $rate_class = $class;
    }
  }
  return 'CLASS_' . str_pad($class, 3, '0');

}
