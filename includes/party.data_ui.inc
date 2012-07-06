<?php
/**
 * @file
 * Provides default classes for UI actions on data sets.
 */

/**
 * The 'add' action: attach a new entity.
 */
class PartyDefaultDataSetUIAdd {

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

  }

  function action_form_validate($form, &$form_state) {

  }

  function action_form_submit($form, &$form_state) {

  }

}

/**
 * The 'attach' action: attach an existing entity.
 */
class PartyDefaultDataSetUIAttach {

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

  }

  function action_form_validate($form, &$form_state) {

  }

  function action_form_submit($form, &$form_state) {

  }

}


