<?php
// $Id$

/**
 * @file
 * Hooks provided by the Cultuurnet API module.
 */

/**
 * Alter the request from a request done using cnapi_get. Mostly used to add some defaults.
 *
 * @param $request
 *   The request for the request.
 */
function hook_cnapi_request_defaults_alter(&$request) {
  $request['pagelength'] = 10;
}

/**
 * Before a request is sent to the API it is validated. By default this only checks the format of the request like:
 * - Is a key provided?
 * - Is a valid action and type provided?
 * - Are valid request filters provided in case of listings and reports?
 *
 * Using this hook you can add extra validation like for example limiting the allowed filters, only allow certain keys, ...
 *
 * @param $request
 *   The request for the request.
 *
 * @return
 *   A boolean indicated if the $request are is valid or not.
 */
function hook_cnapi_request_validate($request) {
  // maximum pagelength is 20
  if (isset($request['query']['pagelength']) && $request['query']['pagelength'] > 20) {
    return FALSE;
  }
}