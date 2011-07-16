<?php

/**
 * Menu callback handler for the object detail page (actor, event, production).
 */
function cnapi_ui_page_detail($object) {
  return array(
    '#theme' => 'cnapi_ui_' . $object['type'],
    '#' . $object['type'] => $object,
  );
}

/**
 * Menu callback handler for the list (context) pages (actor, event, production).
 */
function cnapi_ui_page_list() {
  // Getting the current request as a Drupal request object.
  $request_dp = cnapi_ui_get_active_request();

  // Converting it to an API request.
  $request = cnapi_url_dp2p($request_dp);

  // Doing the API request.
  $result = cnapi_get($request);

  // Fetch the total from the result and default to 0.
  $total = isset($result['total']) ? $result['total'] : 0;

  // Fetch the data from the result.
  $data = isset($result['data']) ? $result['data'] : array();

  // Add the API defaults to get the pagelength.
  cnapi_request_add_defaults($request);
  $pagelength = $request['query']['pagelength'];

  // Initialize the Drupal pager.
  pager_default_initialize($total, $pagelength);

  // Get the full context info for the current request.
  $context = cnapi_ui_contexts('id', $request_dp['context']);

  // Add the RSS feed url to the <head>.
  drupal_add_feed(cnapi_ui_rss_feed_url(), cnapi_ui_page_list_title());

  // Return the result as a render array using theme_cnapi_ui_list.
  return array(
    '#theme' => 'cnapi_ui_list',
    '#type' => $context['type'],
    '#items' => $data,
    '#total' => $total,
    '#request' => $request_dp
  );
}

/**
 * Menu callback handler for the RSS feeds.
 */
function cnapi_ui_page_list_rss() {
  // Get the path for the current request. We get this by looking at the Drupal path ($_GET['q']) and stripping of the 'rss/' prefix.
  $path = substr($_GET['q'], strlen('rss/'), strlen($_GET['q']));

  // The $query for the current request is the whole $_GET array without $_GET['q'] (the path).
  $query = $_GET;
  unset($query['q']);

  // Clean invalid query parameters that the API won't understand (or that we don't allow) like utm_source, ...
  cnapi_ui_clean_invalid_queries($query);

  // Clean query paramters that don't matter for RSS.
  cnapi_ui_clean_rss_queries($query);

  // Set the pagelength for the API call to the default feed length.
  $query['pagelength'] = variable_get('feed_default_items', 10);

  // Set the sort field and direction to "created DESC".
  $query['sort'] = 'created DESC';

  // Construct the full path...
  $url = array(
    'path' => $path,
    'options' => array(
      'query' => $query,
    ),
  );

  // ... and convert it to an API request ...
  $request = cnapi_url_dua2p($url);

  // ... and also convert it to a Drupal request.
  $request_dp = cnapi_url_dua2dp($url);

  // Do the API call.
  $result = cnapi_get($request);

  // Fetch the total from the result and default to 0.
  $total = isset($result['total']) ? $result['total'] : 0;

  // Fetch the data from the result.
  $data = isset($result['data']) ? $result['data'] : array();

  // Get the full context info for the current request.
  $context = cnapi_ui_contexts('id', $request_dp['context']);

  // Add the RSS content-type header.
  drupal_add_http_header('Content-Type', 'application/rss+xml; charset=utf-8');

  // Return the themed feed as a render array.
  print theme('cnapi_ui_page_list_rss', array('objects' => $data, 'type' => $request_dp['context']));
}