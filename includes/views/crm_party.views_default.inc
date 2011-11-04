<?php
/**
 * @file crm_party.views_default.inc
 * Provides default views for the CRM Party module.
 */

/**
 * Implements hook_views_default_views().
 */
function crm_party_views_default_views() {
  $view = new view;
  $view->name = 'crm_party_attached_entities';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'crm_party';
  $view->human_name = 'crm_party_attached_entities';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['access']['type'] = 'none';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['query_comment'] = FALSE;
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'none';
  $handler->display->display_options['style_plugin'] = 'list';
  $handler->display->display_options['row_plugin'] = 'party_attached_entity';
  /* Field: Party: Party ID */
  $handler->display->display_options['fields']['pid']['id'] = 'pid';
  $handler->display->display_options['fields']['pid']['table'] = 'crm_party';
  $handler->display->display_options['fields']['pid']['field'] = 'pid';
  /* Contextual filter: Party: Party ID */
  $handler->display->display_options['arguments']['pid']['id'] = 'pid';
  $handler->display->display_options['arguments']['pid']['table'] = 'crm_party';
  $handler->display->display_options['arguments']['pid']['field'] = 'pid';
  $handler->display->display_options['arguments']['pid']['default_action'] = 'not found';
  $handler->display->display_options['arguments']['pid']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['pid']['default_argument_skip_url'] = 0;
  $handler->display->display_options['arguments']['pid']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['pid']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['pid']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['pid']['break_phrase'] = 0;
  $handler->display->display_options['arguments']['pid']['not'] = 0;
  /* Filter criterion: Party: Attached entity bundle */
  $handler->display->display_options['filters']['entity_bundle']['id'] = 'entity_bundle';
  $handler->display->display_options['filters']['entity_bundle']['table'] = 'crm_party_attached_entity';
  $handler->display->display_options['filters']['entity_bundle']['field'] = 'entity_bundle';
  /* Filter criterion: Party: Attached entity type */
  $handler->display->display_options['filters']['entity_type']['id'] = 'entity_type';
  $handler->display->display_options['filters']['entity_type']['table'] = 'crm_party_attached_entity';
  $handler->display->display_options['filters']['entity_type']['field'] = 'entity_type';

  // Get all defined data sets.
  $sets = crm_party_get_data_set_info();
  // Make a display for each data set.
  foreach ($sets as $set_id => $set) {
    $handler = $view->new_display('crm_party_piece', $set['label'], 'crm_party_piece_' . $set_id);
    $handler->display->display_options['display_description'] = 'Party component - ' . $set['label'];
    $handler->display->display_options['defaults']['filters'] = FALSE;
    /* Filter criterion: Party: Attached entity type */
    $handler->display->display_options['filters']['entity_type']['id'] = 'entity_type';
    $handler->display->display_options['filters']['entity_type']['table'] = 'crm_party_attached_entity';
    $handler->display->display_options['filters']['entity_type']['field'] = 'entity_type';
    $handler->display->display_options['filters']['entity_type']['value'] = $set['entity type'];
    $handler->display->display_options['filters']['entity_type']['group'] = 0;

    if (isset($set['entity bundle'])) {
      /* Filter criterion: Party: Attached entity bundle */
      $handler->display->display_options['filters']['entity_bundle']['id'] = 'entity_bundle';
      $handler->display->display_options['filters']['entity_bundle']['table'] = 'crm_party_attached_entity';
      $handler->display->display_options['filters']['entity_bundle']['field'] = 'entity_bundle';
      $handler->display->display_options['filters']['entity_bundle']['value'] = $set['entity bundle'];
      $handler->display->display_options['filters']['entity_bundle']['group'] = 0;
    }
  }

  $views[$view->name] = $view;

  return $views;
}
