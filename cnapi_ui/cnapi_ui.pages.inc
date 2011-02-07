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
  
  $pool = array(
    'name' => 'The name of the pool',
    'domain' => array('www.somedomain.com', 'www.someotherdomain.net'),
    'type' => 1,
    'filters' => array(
      'categories' => array('include' => array('1.0.0.0.0'), 'exclude' => array('1.1.0.0.0')),
      'keywords' => array('include' => array('do include'), 'exclude' => array('do exclude')),
    ),
  );
  
  print "<pre>".print_r(cnapi_mgt_edit_userpool($pool), TRUE)."</pre>";

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

function cnapi_ui_page_productions() {
  $items = array();

  $request = cnapi_ui_get_request();

  $productions = cnapi_get_productions($request['query']);
  foreach ($productions['data'] as $production) {
    $items[] = theme('cnapi_ui_production_summary', array('production' => $production));
  }

  pager_default_initialize($productions['total'], $request['query']['pagelength']);

  return array(
    'number_of_productions' => array(
      '#markup' => t('!total productions found', array('!total' => $productions['total'])),
    ),
    'productions' => array(
      '#markup' => theme('item_list', array('items' => $items, 'attributes' => array('class' => 'production-summary-list'))),
    ),
    'pager' => array(
      '#theme' => 'pager',
    ),
  );
}

function cnapi_ui_page_actors() {
  $items = array();

  $request = cnapi_ui_get_request();

  $actors = cnapi_get_actors($request['query']);
  foreach ($actors['data'] as $actor) {
    $items[] = theme('cnapi_ui_actor_summary', array('actor' => $actor));
  }

  pager_default_initialize($actors['total'], $request['query']['pagelength']);

  return array(
    'number_of_actors' => array(
      '#markup' => t('!total actors found', array('!total' => $actors['total'])),
    ),
    'actors' => array(
      '#markup' => theme('item_list', array('items' => $items, 'attributes' => array('class' => 'actor-summary-list'))),
    ),
    'pager' => array(
      '#theme' => 'pager',
    ),
  );
}