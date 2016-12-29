<?php

/**
 * @file
 * Admin functions for Commerce FedEx Freight service.
 */

/**
 * Builds the admin settings form for configuring FedEx Freight.
 *
 * @return array
 *   Drupal form for FedEx Freight service settings.
 */
function commerce_fedex_freight_settings_form() {
  $request_mode = variable_get('commerce_fedex_freight_request_mode', 'testing');
  $form = array();
  $form['auth'] = array(
    '#title' => t('FedEx web authentication'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
  );
  $form['auth']['commerce_fedex_freight_request_mode'] = array(
    '#type' => 'radios',
    '#title' => t('FedEx Freight Request Mode'),
    '#description' => t('If switching to production mode, make sure that you have acquired production credentials from FedEx. These will be different than your testing credentials.'),
    '#options' => array(
      'testing' => t('Testing'),
      'production' => t('Production'),
    ),
    '#default_value' => $request_mode,
  );
  $form['auth']['production'] = array(
    '#title' => t('Production Settings'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => $request_mode == 'production' ? FALSE : TRUE,
    '#states' => array(
      'expanded' => array(
        ':input[name=commerce_fedex_freight_request_mode]' => array('value' => 'production'),
      ),
    ),
  );
  $form['auth']['production']['commerce_fedex_freight_reg_key'] = array(
    '#type' => 'textfield',
    '#title' => t('FedEx Freight Registration Key'),
    '#default_value' => variable_get('commerce_fedex_freight_reg_key'),
    '#required' => $request_mode == 'production' ? TRUE : FALSE,
    '#states' => array(
      'required' => array(
        ':input[name=commerce_fedex_freight_request_mode]' => array('value' => 'production'),
      )
    )
  );
  $form['auth']['production']['commerce_fedex_freight_account_number'] = array(
    '#type' => 'textfield',
    '#title' => t('FedEx Freight Account Number'),
    '#default_value' => variable_get('commerce_fedex_freight_account_number'),
  );
  $form['auth']['testing'] = array(
    '#title' => t('Test Settings'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => !$form['auth']['production']['#collapsed'],
    '#states' => array(
      'expanded' => array(
        ':input[name=commerce_fedex_freight_request_mode]' => array('value' => 'testing')
      ),
    ),
  );
  $form['auth']['testing']['commerce_fedex_freight_reg_key_testing'] = array(
    '#type' => 'textfield',
    '#title' => t('FedEx Key'),
    '#default_value' => variable_get('commerce_fedex_freight_reg_key_testing'),
  );
  $form['auth']['testing']['commerce_fedex_freight_account_number_testing'] = array(
    '#type' => 'textfield',
    '#title' => t('FedEx Account Number'),
    '#default_value' => variable_get('commerce_fedex_freight_account_number_testing'),
  );

  foreach (element_children($form['auth']) as $mode) {
    foreach (element_children($form['auth'][$mode]) as $field_name) {
      $form['auth'][$mode][$field_name] += array(
        '#states' => array(
          'required' => array(
            ':input[name=commerce_fedex_freight_request_mode]' => array('value' => $mode),
          ),
        ),
      );
    }
  }

  $form['origin'] = array(
    '#type' => 'fieldset',
    '#title' => t('Ship From Address'),
    '#collapsible' => TRUE,
  );
  $form['origin']['commerce_fedex_freight_factor'] = array(
    '#type' => 'textfield',
    '#title' => t('Freight Factor'),
    '#default_value' => variable_get('commerce_fedex_freight_factor'),
  );
  $form['origin']['commerce_fedex_freight_shipper_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => variable_get('commerce_fedex_freight_shipper_name'),
  );
  $form['origin']['commerce_fedex_freight_company_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Company Name'),
    '#default_value' => variable_get('commerce_fedex_freight_company_name'),
  );
  $form['origin']['commerce_fedex_freight_address_line_1'] = array(
    '#type' => 'textfield',
    '#title' => t('Address'),
    '#required' => TRUE,
    '#default_value' => variable_get('commerce_fedex_freight_address_line_1'),
  );
  $form['origin']['commerce_fedex_freight_address_line_2'] = array(
    '#type' => 'textfield',
    '#title' => t('Address (Additional)'),
    '#default_value' => variable_get('commerce_fedex_freight_address_line_2'),
  );
  $form['origin']['commerce_fedex_freight_city'] = array(
    '#type' => 'textfield',
    '#title' => t('City'),
    '#required' => TRUE,
    '#default_value' => variable_get('commerce_fedex_freight_city'),
  );
  $form['origin']['commerce_fedex_freight_state'] = array(
    '#type' => 'textfield',
    '#title' => t('State or Province'),
    '#description' => t('If shipping from USA or Canada, enter the 2 character abbreviation for the shipping State or Province.'),
    '#default_value' => variable_get('commerce_fedex_freight_state'),
    '#size' => 2,
  );
  $form['origin']['commerce_fedex_freight_postal_code'] = array(
    '#type' => 'textfield',
    '#title' => t('Postal Code'),
    '#description' => t('Enter your postal code if available.'),
    '#size' => 12,
    '#default_value' => variable_get('commerce_fedex_freight_postal_code'),
  );
  $form['origin']['commerce_fedex_freight_country_code'] = array(
    '#type' => 'select',
    '#title' => t('Country'),
    '#default_value' => variable_get('commerce_fedex_freight_country_code'),
    '#options' => array('' => t('Select Country')) + commerce_fedex_serviced_countries(),
    '#required' => TRUE,
  );
  $form['freight_services'] = array(
    '#type' => 'fieldset',
    '#title' => t('Enabled FedEx Freight Shipping Services'),
    '#collapsible' => TRUE,
  );
  $form['freight_services']['commerce_fedex_freight_services'] = array(
    '#type' => 'checkboxes',
    '#options' => commerce_fedex_freight_shipping_service_types(),
    '#default_value' => variable_get('commerce_fedex_freight_services', array()),
  );
  $form['options']['commerce_fedex_freight_shipto_residential'] = array(
    '#type' => 'select',
    '#title' => t('Ship to destination type'),
    '#description' => t('Leave this set as residential unless you only ship to commercial addresses.'),
    '#options' => array(
      'residential' => t('Residential'),
      'commercial' => t('Commercial')
    ),
    '#default_value' => variable_get('commerce_fedex_freight_shipto_residential', 'residential'),
  );
  $form['options']['commerce_fedex_freight_show_logo'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display FedEx Logo next to FedEx services.'),
    '#default_value' => variable_get('commerce_fedex_freight_show_logo', 0),
  );
  $form['options']['commerce_fedex_freight_log'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Log the API Request and Response for debugging'),
    '#options' => array(
      'response' => t('API request & response messages'),
    ),
    '#default_value' => variable_get('commerce_fedex_freight_log', 0),
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );
  return $form;
}

/**
 * Validate handler for FedEx Freight admin settings form.
 *
 * @param array $form
 *   The FedEx admin settings form.
 * @param array $form_state
 *   The FedEx admin settings form state.
 */
function commerce_fedex_freight_settings_form_validate($form, $form_state) {

  if ("" == (trim($form_state['values']['commerce_fedex_freight_shipper_name'])) && "" == (trim($form_state['values']['commerce_fedex_freight_company_name']))) {
    form_set_error('commerce_fedex_freight_shipper_name', t('You must enter one or both of the Person name or Company name fields.'));
  }

  // Validate FedEx settings that are required from USA and Canada only.
  if (in_array($form_state['values']['commerce_fedex_freight_country_code'], array('US', 'CA'))) {

    // Make sure state is provided.
    if (empty($form_state['values']['commerce_fedex_freight_state'])) {
      form_set_error('commerce_fedex_freight_state', t('State or Province is required when shipping from USA or Canada.'));
    }
    else {

      // Make sure state is 2 characters.
      if (!preg_match('/^[a-z][a-z]$/i', $form_state['values']['commerce_fedex_freight_state'])) {
        form_set_error('commerce_fedex_freight_state', t('Please enter the 2 character abbreviation for your State or Province'));
      }
    }

    // Make sure postal code is provided.
    if (empty($form_state['values']['commerce_fedex_freight_postal_code'])) {
      form_set_error('commerce_fedex_freight_postal_code', t('Postal code is required when shipping from USA or Canada.'));
    }
  }
}

/**
 * Submit handler for FedEx Freight admin settings form.
 *
 * @param array $form
 *   The FedEx Freight admin settings form.
 * @param array $form_state
 *   The FedEx Freight admin settings form state.
 */
function commerce_fedex_freight_settings_form_submit($form, $form_state) {
    // Make sure State/Province value is saved as uppercase.
  if (!empty($form_state['values']['commerce_fedex_freight_state'])) {
    $form_state['values']['commerce_fedex_freight_state'] = drupal_strtoupper($form_state['values']['commerce_fedex_freight_state']);
  }

  // Loop through each of the settings fields and save their values.
  foreach (commerce_fedex_freight_settings_fields() as $key) {
    if (!empty($form_state['values'][$key])) {
      variable_set($key, $form_state['values'][$key]);
    }
  }

  drupal_set_message(t('The FedEx Freight configuration options have been saved.'));
}
