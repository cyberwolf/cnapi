<?php
// $Id$

function cnapi_ui_page_events() {
  $items = array();
  
  $request = cnapi_ui_get_request();

  $events = cnapi_get_events($request['query']);
  foreach ($events['data'] as $event) {
    $items[] = theme('cnapi_ui_event_summary', array('event' => $event));
  }
  
  pager_default_initialize($events['total'], $request['query']['pagelength']);

  return array(
    'number_of_events' => array(
      '#markup' => t('!total events found', array('!total' => $events['total'])),
    ),
    'events' => array(
      '#markup' => theme('item_list', array('items' => $items, 'attributes' => array('class' => 'event-summary-list'))),
    ),
    'pager' => array(
      '#theme' => 'pager',
    ),
  );
}