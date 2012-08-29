<?php
/**
 * @file
 * Provides default classes for UI actions on data sets.
 */

/**
 * The interface for action UI controllers.
 *
 * @see party_attached_entity_action_form()
 */
interface PartyDataSetActionInterface {

  /**
   * Get the page title for the action form.
   *
   * @todo: consider moving this to the data set action definition, where the
   * local action label is already defined.
   *
   * @param $party
   *   A loaded party object.
   * @param $data_set
   *   A loaded data set.
   *   We don't actually need this loaded, but we need a menu loader to convert
   *   the path-style string to the machine name, and a menu loader that doesn't
   *   load would be weird too.
   * @param $eid
   *   The id of the entity, if relevant to this action.
   *
   * @return
   *  A localized string.
   */
  public function get_page_title($name, $var);

  /**
   * Provides the action form for adding a new entity.
   *
   * @param $party
   *   A loaded party object.
   * @param $data_set
   *   A loaded data set.
   *   We don't actually need this loaded, but we need a menu loader to convert
   *   the path-style string to the machine name, and a menu loader that doesn't
   *   load would be weird too.
   * @param $eid
   *   The id of the entity, if relevant to this action.
   *
   * @return
   *  A FormAPI array.
   */
  function action_form($form, &$form_state, $party, $data_set, $eid = NULL);

  /**
   * Form validation for the action form.
   */
  function action_form_validate($form, &$form_state);

  /**
   * Form submission for the action form.
   */
  function action_form_submit($form, &$form_state);

}

/**
 * The 'add' action: attach a new entity.
 *
 * @todo: not yet in use. See todo in party_menu().
 */
class PartyDefaultDataSetUIAdd implements PartyDataSetActionInterface {

  /**
   * The page title for the action form.
   */
  function get_page_title($party, $data_set, $eid = NULL) {
    $data_set_controller = party_get_crm_controller($party, $data_set['set_name']);
    return t("Add new @data-set-label to @party-label", array(
      '@data-set-label' => $data_set_controller->getDataInfo('label'),
      '@party-label' => $party->label,
    ));
  }

  /**
   * Provides the action form for adding a new entity.
   *
   * @param $party
   *   A loaded party object.
   * @param $data_set
   *   A loaded data set.
   *   We don't actually need this loaded, but we need a menu loader to convert
   *   the path-style string to the machine name, and a menu loader that doesn't
   *   load would be weird too.
   * @param $eid
   *   The id of the entity, if relevant to this action.
   */
  function action_form($form, &$form_state, $party, $data_set, $eid = NULL) {
    $form = array();

    // @todo: this is not in use yet.

    return $form;
  }

  function action_form_validate($form, &$form_state) {

  }

  function action_form_submit($form, &$form_state) {

  }

}

/**
 * The 'attach' action: attach an existing entity.
 */
class PartyDefaultDataSetUIAttach implements PartyDataSetActionInterface {

  /**
   * The page title for the action form.
   */
  function get_page_title($party, $data_set, $eid = NULL) {
    $data_set_controller = party_get_crm_controller($party, $data_set['set_name']);
    return t("Attach existing @data-set-label to @party-label", array(
      '@data-set-label' => $data_set_controller->getDataInfo('label'),
      '@party-label' => $party->label,
    ));
  }

  /**
   * Provides the action form for attaching an existing entity.
   *
   * @param $party
   *   A loaded party object.
   * @param $data_set
   *   A loaded data set.
   *   We don't actually need this loaded, but we need a menu loader to convert
   *   the path-style string to the machine name, and a menu loader that doesn't
   *   load would be weird too.
   * @param $eid
   *   The id of the entity, if relevant to this action.
   */
  function action_form($form, &$form_state, $party, $data_set, $eid = NULL) {
    $form = array();

    dsm("This doesn't work for general entities yet!");

    return $form;
  }

  function action_form_validate($form, &$form_state) {

  }

  function action_form_submit($form, &$form_state) {

  }

}
