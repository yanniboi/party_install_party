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
 *
 * Properties:
 *  - hid
 *  - name
 *  - label
 *  - data: An array of various data for the hat:
 *    - party_types: OBSOLETE, to be removed.
 *    - data_sets: TODO.
 *  - required
 *  - status
 *  - module
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

/**
 * Extends the Party UI controller to show a party's hats in the admin list.
 */
class PartyHatPartyUIController extends PartyUIController {

  /**
   * Generates the table headers for the overview table.
   */
  protected function overviewTableHeaders($conditions, $rows, $additional_header = array()) {
    // Get the basic row from the parent class.
    $header = parent::overviewTableHeaders($conditions, $rows, $additional_header);

    array_splice($header, 2, 0, t('Hats'));

    return $header;
  }

  protected function overviewTableRow($conditions, $id, $party, $additional_cols = array()) {
    // Get the basic row from the parent class.
    $row = parent::overviewTableRow($conditions, $id, $party, $additional_cols);

    $hats_labels = array();
    $hats = party_hat_get_hats($party);
    foreach ($hats as $hat) {
      $hats_labels[] = $hat->label;
    }
    if (count($hats_labels)) {
      $hats_text = check_plain(implode(', ', $hats_labels));
    }
    else {
      $hats_text = t('None');
    }

    array_splice($row, 2, 0, $hats_text);

    return $row;
  }

}
