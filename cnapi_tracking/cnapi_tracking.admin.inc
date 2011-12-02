<?php

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
function cnapi_tracking_admin_settings() {
  // general api settings
  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Configuration'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['settings']['cnapi_tracking_script_location'] = array(
    '#type' => 'textfield',
    '#title' => t('CultuurNet API tracking script location'),
    '#description' => t('The URL where the CultuurNet API tracking script resides.'),
    '#default_value' => variable_get('cnapi_tracking_script_location', CNAPI_TRACKING_SCRIPT_LOCATION),
  );

  return system_settings_form($form);
}