<?php

/**
 * Make a request to the CultuurNet API using the defaults added by this module and others. The request will also be cleaned to remove redundant parameters before firing off the request.
 *
 * @param $request
 *   A CNAPI request array (@see _cnapi_get) representing the request.
 *
 * @return
 *   An array representing the answer (@see _cnapi_get) from the CultuurNet API in case of success. FALSE in case an error occured.
 */
function cnapi_get($request) {
  cnapi_request_add_defaults($request);
  cnapi_request_clean($request);

  return _cnapi_get($request);
}

/**
 * Make a request to the CultuurNet API.
 *
 * @param $request
 *   A CNAPI request array (@see _cnapi_get) representing the request.
 *
 * @return
 *   An array representing the answer (@see _cnapi_get) from the CultuurNet API in case of success. FALSE in case an error occured.
 */
function cnapi_get_raw($request) {
  return _cnapi_get($request);
}

/**
 * Request an event from the the CultuurNet API.
 *
 * @param $cdib
 *   The CDBID of the requested event.
 * @param $related_events
 *   Specifies if events related to the requested event should be fetched too and how these should be formatted. The options are:
 *     - CNAPI_LIST_IGNORE : Do not fetch related events.
 *     - CNAPI_LIST_SUMMARY : Fetch a summary of all related events.
 *     - CNAPI_LIST_DETAIL : Fetch the full info of related events.
 *     - CNAPI_LIST_ID : Fetch the CDBIDs of related events.
 *
 * @return
 *   An array representing the event (@see _cnapi_get) from the CultuurNet API in case of success. FALSE in case an error occured.
 */
function cnapi_get_event($cdbid, $related_events = CNAPI_LIST_IGNORE) {
  $request = array('action' => 'detail', 'type' => 'event', 'query' => array('cdbid' => $cdbid));
  if ($related_events != CNAPI_LIST_IGNORE) {
    $request['query']['relatedevents'] = ($related_events == CNAPI_LIST_SUMMARY ? 'list' : 'detail');
  }
  return cnapi_get($request);
}

/**
 * Request a production from the the CultuurNet API.
 *
 * @param $cdib
 *   The CDBID of the requested production.
 * @param $related_events
 *   Specifies if events related to the requested production should be fetched too and how these should be formatted. The options are:
 *     - CNAPI_LIST_IGNORE : Do not fetch related events.
 *     - CNAPI_LIST_SUMMARY : Fetch a summary of all related events.
 *     - CNAPI_LIST_DETAIL : Fetch the full info of related events.
 *     - CNAPI_LIST_ID : Fetch the CDBIDs of related events.
 *
 * @return
 *   An array representing the production (@see _cnapi_get) from the CultuurNet API in case of success. FALSE in case an error occured.
 */
function cnapi_get_production($cdbid, $related_events = CNAPI_LIST_IGNORE) {
  $request = array('action' => 'detail', 'type' => 'production', 'query' => array('cdbid' => $cdbid));
  if ($related_events != CNAPI_LIST_IGNORE) {
    $request['query']['relatedevents'] = ($related_events == CNAPI_LIST_SUMMARY ? 'list' : 'detail');
  }

  return cnapi_get($request);
}

/**
 * Request an actor from the the CultuurNet API.
 *
 * @param $cdib
 *   The CDBID of the requested actor.
 *
 * @return
 *   An array representing the actor (@see _cnapi_get) from the CultuurNet API in case of success. FALSE in case an error occured.
 */
function cnapi_get_actor($cdbid) {
  $request = array('action' => 'detail', 'type' => 'actor', 'query' => array('cdbid' => $cdbid));
  return cnapi_get($request);
}

function cnapi_get_events($query = NULL, $action = CNAPI_LIST_SUMMARY) {
  $request = array('action' => ($action == CNAPI_LIST_SUMMARY ? 'list_summary' : 'list_detail'), 'type' => 'event', 'query' => $query);

  return cnapi_get($request);
}

function cnapi_get_productions($query = NULL, $action = CNAPI_LIST_SUMMARY) {
  $request = array('action' => ($action == CNAPI_LIST_SUMMARY ? 'list_summary' : 'list_detail'), 'type' => 'production', 'query' => $query);

  return cnapi_get($request);
}

function cnapi_get_actors($query = NULL, $action = CNAPI_LIST_SUMMARY) {
  $request = array('action' => ($action == CNAPI_LIST_SUMMARY ? 'list_summary' : 'list_detail'), 'type' => 'actor', 'query' => $query);
  return cnapi_get($request);
}

function cnapi_get_report($type, $query = array()) {
  $request = array('action' => 'report', 'type' => $type, 'query' => $query);
  return cnapi_get($request);
}

function _cnapi_get($request) {
  if (!cnapi_request_validate($request)) {
    watchdog('cnapi', 'Invalid API request !request.', array('!request' => serialize($request)), WATCHDOG_ERROR);
    return FALSE;
  }

  $result = NULL;

  $cache = _cnapi_cache_get($request);

  if ($cache != NULL) {
    $result = $cache;
  }
  else {
    $result = cnapi_api_request($request);

    if ($result) {
      $result = _cnapi_parse($request, $result);

      if ($result) {
        if (in_array($request['action'], array('list_detail', 'list_summary'))) {
          _cnapi_cache_list_clean_by_total($request, $result['total']);
        }

        if ($request['action'] != 'detail') { // detail actions will be cached at parse level
          _cnapi_cache_set($request, $result);
        }
      }
      else {
        watchdog('cnapi', 'Failed parsing for request !request.', array('!request' => serialize($request)), WATCHDOG_ERROR);
        return FALSE;
      }
    }
    else {
      watchdog('cnapi', 'Failed request !request.', array('!request' => serialize($request)), WATCHDOG_ERROR);
      return FALSE;
    }
  }

  return $result;
}

function cnapi_api_request($request) {
  if (!isset($request['query']['format'])) {
    $request['query']['format'] = 'xml';
  }
  $url = cnapi_url_p2a($request);
  $url = rtrim(variable_get('cnapi_api_location', CNAPI_API_LOCATION), '/') . '/api/' . $url;
  return cnapi_http_request($url);
}