<?php
/**
 * Views plugin for party pieces.
 */
class crm_party_plugin_display_party_piece extends views_plugin_display {
  function option_definition () {
    $options = parent::option_definition();

    //$options['formatter_label'] = array('default' => '', 'translatable' => TRUE);

    return $options;
  }
  
  /**
   * Provide the default form for setting options.
   */
  function options_form(&$form, &$form_state) {
    // It is very important to call the parent function here:
    parent::options_form($form, $form_state);
  }

  /**
   * metacribbed from eva.
   * The display block handler returns the structure necessary for a block.
   */
  function execute() {
    // Prior to this being called, the $view should already be set to this
    // display, and arguments should be set on the view.
    if (!isset($this->view->override_path)) {
      $this->view->override_path = $_GET['q'];
    }

    $data = $this->view->render();
    if (!empty($this->view->result) || $this->get_option('empty') || !empty($this->view->style_plugin->definition['even empty'])) {
      return $data;
    }
  }

  /**
   * Returns items for hook_crm_party_party_pieces().
   *
   * This is the same pattern as the block display plugin uses. It's neat ;)
   */
  function execute_hook_crm_party_party_pieces() {
    $key = 'views' . '_' . $this->view->name . '_' . $this->display->id;

    $pieces = array(
      $key => array(
        'title' => 'View', // todo: lift this from the title for the display or an option
        'page callback' => 'crm_party_page_view_piece_views',
        // We need to be able to find this view in the page callback.
        'page arguments' => array(1, $this->view->name, $this->display->id),
        'file' => 'crm_party.pages.inc',
        'access arguments' => array('view contacts'), // todo.
      ),
    );

    return $pieces;
  }
}
