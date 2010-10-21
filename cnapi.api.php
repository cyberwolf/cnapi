<?php
// $Id$

/**
 * Alter the params from a request done using cnapi_get. Mostly used to add some defaults.
 *
 * @param $params
 *   The params for the request.
 */
function hook_cnapi_params_defaults_alter(&$params) {
  $params['pagelength'] = 10;
}

/**
 * Before a request is sent to the API it is validated. By default this only checks the format of the $params like:
 * - Is a key provided?
 * - Is a valid action and type provided?
 * - Are valid request filters provided in case of listings and reports?
 *
 * Using this hook you can add extra validation like for example limiting the allowed filters, only allow certain keys, ...
 *
 * @param $params
 *   The params for the request.
 *
 * @return
 *   A boolean indicated if the $params are is valid or not.
 */
function hook_cnapi_params_validate($params) {
  // maximum pagelength is 20
  if (isset($params['query']['pagelength']) && $params['query']['pagelength'] > 20) {
    return FALSE;
  }
}