<?php

function cnapi_ui_page_detail($object) {
  return array(
    '#theme' => 'cnapi_ui_' . $object['type'],
    '#' . $object['type'] => $object,
  );
}

function cnapi_ui_page_list() {
  $request_dp = cnapi_ui_get_active_request();

  $request = cnapi_url_dp2p($request_dp);

  $result = cnapi_get($request);

  $total = isset($result['total']) ? $result['total'] : 0;
  $data = isset($result['data']) ? $result['data'] : array();

  cnapi_request_add_defaults($request);
  $pagelength = $request['query']['pagelength'];

  pager_default_initialize($total, $pagelength);

  $context = cnapi_ui_contexts('id', $request_dp['context']);

  drupal_add_feed(cnapi_ui_rss_feed_url(), cnapi_ui_page_list_title());

  return theme('cnapi_ui_list', array('type' => $context['type'], 'items' => $data, 'total' => $total, 'request' => $request_dp)); // @todo add hints for type
}

function cnapi_ui_page_list_rss() {
  $path = substr($_GET['q'], strlen('rss/'), strlen($_GET['q']));
  $query = $_GET;
  unset($query['q']);

  cnapi_ui_clean_invalid_queries($query);
  cnapi_ui_clean_rss_queries($query);

  $query['pagelength'] = variable_get('feed_default_items', 10);
  $query['sort'] = 'created DESC';

  $url = array(
    'path' => $path,
    'options' => array(
      'query' => $query,
    ),
  );

  $request = cnapi_url_dua2p($url);

  $request_dp = cnapi_url_dua2dp($url);

  $result = cnapi_get($request);

  $total = isset($result['total']) ? $result['total'] : 0;
  $data = isset($result['data']) ? $result['data'] : array();

  $context = cnapi_ui_contexts('id', $request_dp['context']);

  drupal_add_http_header('Content-Type', 'application/rss+xml; charset=utf-8');

  print theme('cnapi_ui_page_list_rss', array('objects' => $data, 'type' => $request_dp['context']));
}