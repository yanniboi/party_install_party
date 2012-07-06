<?php
/**
 * @file
 * Provides classes for UI actions on the user data set.
 */

/**
 * The 'attach' action: attach an existing entity.
 */
class PartyUserDataSetUIAttach extends PartyDefaultDataSetUIAttach {

  /**
   * Provides the action form for attaching an existing user.
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


