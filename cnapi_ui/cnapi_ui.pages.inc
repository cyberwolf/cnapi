<?php

function cnapi_ui_page_list() {
  $request_dp = cnapi_ui_get_active_request();

  $request = cnapi_url_dp2p($request_dp);

  $result = cnapi_get($request);

  $total = $result['total'];
  $data = $result['data'];

  cnapi_request_add_defaults($request);
  $pagelength = $request['query']['pagelength'];

  pager_default_initialize($total, $pagelength);

  $context = cnapi_ui_contexts('id', $request_dp['context']);

  $items = array();
  if ($data) {
    foreach ($data as $object) {
      $items[] = theme('cnapi_ui_' . $context['type'] . '_summary', array($context['type'] => $object));
    }
  }

  return array(
    'items' => array(
      '#theme' => 'item_list',
      '#items' => $items,
    ),
    'pager' => array(
      '#theme' => 'pager',
    )
  );
}