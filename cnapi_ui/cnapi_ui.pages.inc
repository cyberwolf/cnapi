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
  
  // mockup : showing data
  $items = array();
  if ($data) {
    foreach ($data as $production) {
      $request_detail = array(
        $context['type'] => $production['cdbid'],
        'title' => $production['title'],
      );
      $items[] = cnapi_url_dp2dul($production['title'], $request_detail);
    }
  }
  
  return theme('item_list', array('items' => $items)) . theme('pager');
}