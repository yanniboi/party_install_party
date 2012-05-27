<?php
/**
 * @file
 * Contains the controller classes for Party entities.
 */

/**
 * The API controller class for the Party Hat entity.
 */
class PartyHatController extends EntityAPIControllerExportable {

}

/**
 * The class used for hat entities.
 */
class PartyHat extends Entity {
  public function __construct($values = array()) {
    parent::__construct($values, 'party_hat');
  }

  protected function defaultLabel() {
    return $this->label;
  }
}

/**
 * UI controller.
 */
class PartyHatUIController extends EntityDefaultUIController {

  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path]['title'] = 'Manage hats';
    // Set the main hat admin page as a local task so it appears within the
    // main Community admin area.
    $items[$this->path]['type'] = MENU_LOCAL_TASK;

    // Disambiguate the 'list' tab.
    $items[$this->path . '/list']['title'] = 'List hats';

    return $items;
  }
}
