<?php
// $Id$

/**
 * @file
 * Callback and handler implementations for administration pages.
 */

/**
 * Configuration form for general settings.
 *
 * @ingroup forms
 * @see system_settings_form()
 */
function cnapi_admin_settings() {
  // general api settings
  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Configuration'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['settings']['cnapi_api_location'] = array(
    '#type' => 'textfield',
    '#title' => t('API location'),
    '#description' => t('The URL where the Cultuurnet API resides. End with a slash. Example: http://build.uitdatabank.be/'),
    '#default_value' => variable_get('cnapi_api_location', CNAPI_API_LOCATION),
  );
  $form['settings']['cnapi_api_key'] = array(
    '#type' => 'textfield',
    '#title' => t('API key'),
    '#description' => t('Your Cultuurnet API key.'),
    '#default_value' => variable_get('cnapi_api_key', ''),
    '#size' => 40,
    '#maxlength' => 40,
  );
  $form['settings']['cnapi_api_output_type'] = array(
    '#type' => 'textfield',
    '#title' => t('Output type'),
    '#description' => t('Your Cultuurnet API output type.'),
    '#default_value' => variable_get('cnapi_api_output_type', 1),
    '#size' => 2,
    '#maxlength' => 2,
  );

  // proxy settings
  $form['proxy'] = array(
    '#type' => 'fieldset',
    '#title' => t('Proxy server'),
    '#description' => t('If you are connecting over a proxy server, specify its settings here.'),
    '#collapsible' => TRUE,
    '#collapsed' => !variable_get('cnapi_proxy_enabled', FALSE),
  );
  $form['proxy']['cnapi_proxy_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Connect over a proxy server'),
    '#default_value' => variable_get('cnapi_proxy_enabled', FALSE),
  );
  $form['proxy']['cnapi_proxy_server'] = array(
    '#type' => 'textfield',
    '#title' => t('Web proxy server'),
    '#description' => t('The HTTP proxy to tunnel requests through.'),
    '#default_value' => variable_get('cnapi_proxy_server', ''),
    '#states' => array(
      'invisible' => array(
        'input[name="cnapi_proxy_enabled"]' => array('checked' => FALSE),
      ),
    ),
  );
  $form['proxy']['cnapi_proxy_port'] = array(
    '#type' => 'textfield',
    '#title' => t('Port'),
    '#description' => t('Specify the port number of the proxy to connect to. Leave empty for default.'),
    '#default_value' => variable_get('cnapi_proxy_port', ''),
    '#states' => array(
      'invisible' => array(
        'input[name="cnapi_proxy_enabled"]' => array('checked' => FALSE),
      ),
    ),
  );
  $form['proxy']['cnapi_proxy_username'] = array(
    '#type' => 'textfield',
    '#title' => t('Username'),
    '#description' => t('Specify the username for the proxy to connect to. Leave empty for no username.'),
    '#default_value' => variable_get('cnapi_proxy_username', ''),
    '#states' => array(
      'invisible' => array(
        'input[name="cnapi_proxy_enabled"]' => array('checked' => FALSE),
      ),
    ),
  );
  $form['proxy']['cnapi_proxy_password'] = array(
    '#type' => 'textfield',
    '#title' => t('Password'),
    '#description' => t('Specify the password for the proxy to connect to. Leave empty for no password.'),
    '#default_value' => variable_get('cnapi_proxy_password', ''),
    '#states' => array(
      'invisible' => array(
        'input[name="cnapi_proxy_enabled"]' => array('checked' => FALSE),
      ),
    ),
  );

  return system_settings_form($form);
}

/**
 * Configuration form for API defaults.
 *
 * @ingroup forms
 * @see system_settings_form()
 */
function cnapi_admin_defaults() {
  $pagelength_options = drupal_map_assoc(array(5, 10, 15, 20, 25, 50, 100));

  $sort_options = array(
    'date ASC' => t('By date ascending'),
    'date DESC' => t('By date descending'),
    'title ASC' => t('By title ascending'),
    'title DESC' => t('By title descending'),
    'location ASC' => t('By location ascending'),
    'location DESC' => t('By location descending'),
  );

  $contexts = array(
    'general' => array(
      'name' => t('General listings'),
    ),
    'event' => array(
      'name' => t('Event listings'),
    ),
    'production' => array(
      'name' => t('Production listings'),
    ),
    'actor' => array(
      'name' => t('Actor listings'),
    ),
  );

  $form['defaults']['#tree'] = TRUE;

  foreach ($contexts as $context => $info) {
    $context_default_specified = _cnapi_default_values('pagelength', $context) != -1 || _cnapi_default_values('sort', $context) != -1;
    $form['defaults'][$context] = array(
      '#type' => 'fieldset',
      '#title' => $info['name'],
      '#collapsible' => TRUE,
      '#collapsed' => !$context_default_specified,
    );
    $form['defaults'][$context]['pagelength'] = array(
      '#type' => 'select',
      '#title' => t('Items per page'),
      '#description' => t('The amount of items to request by default per API call.'),
      '#options' => array(-1 => t('Use general pagelength settings')) + $pagelength_options,
      '#default_value' => _cnapi_default_values('pagelength', $context),
    );
    $form['defaults'][$context]['sort'] = array(
      '#type' => 'select',
      '#title' => t('Sort'),
      '#description' => t('The amount of items to request by default per API call.'),
      '#options' => array(-1 => t('Use general pagelength settings')) + $sort_options,
      '#default_value' => _cnapi_default_values('sort', $context),
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  return $form;
}

/**
 * Submit handler for configuration form for API defaults.
 */
function cnapi_admin_defaults_submit($form, &$form_state) {
  $defaults = $form_state['values']['defaults'];

  variable_set('cnapi_defaults', $defaults['general']);
  variable_set('cnapi_defaults_events', $defaults['event']);
  variable_set('cnapi_defaults_productions', $defaults['production']);
  variable_set('cnapi_defaults_actors', $defaults['actor']);
  
  drupal_set_message(t('Your defaults have been saved.'));
}