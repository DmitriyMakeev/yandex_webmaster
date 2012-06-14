<?php

/**
 * @file
 * Theming functions for the Yandex.Webmaster module.
 *
 * @ingroup yandex_webmaster
 */

/**
 * Load idna_convert class for correct work with domain-names in punycode.
 */
require_once('idna_convert.class.php');

/**
 * Display table with hosts.
 */
function theme_yandex_webmaster_hosts_table($variables) {
  $form = $variables['form'];
  $rows = array();
  foreach (element_children($form) as $key) {
    $item = $form[$key];
    $row = array();
    // Display radios to select basic host.
    unset($item['enable']['#title']);
    $row[] = array('class' => array('checkbox'), 'data' => drupal_render($item['enable']));
    // Get right hostname.
    $hostname = drupal_render($item['name']);
    $idna = new idna_convert();
    $url = $idna->encode('http://' . $hostname);
    // Display host state as icon.
    $state = drupal_render($item['verification']);
    $virused = drupal_render($item['virused']) == 'false' ? '' : ' VIRUSED';
    $row[] = '<div class="yandex_webmaster_host_state ' . $state . $virused . '">'
      . '<div class="yandex_webmaster_hostname">' . $hostname
      . l($hostname, $url, array(
        'attributes' => array(
          'class' => 'yandex_webmaster_external_link',
          'target' => '_blank',
          'title' => t('Open in new window'),
        ),
      ))
      . '<br />'
      . '<span>'
      . yandex_webmaster_state_description($state, 'verification') . ' '
      . yandex_webmaster_state_description(drupal_render($item['crawling']), 'index')
      . '</span> '
      . '</div>';
    $row[] = drupal_render($item['tcy']);
    $value = drupal_render($item['url-count']);
    $row[] = $value > 0 ? number_format($value, 0, '', ' ') : $value;
    $value = drupal_render($item['index-count']);
    $row[] = $value > 0 ? number_format($value, 0, '', ' ') : $value;
    $date = drupal_render($item['last-access']);
    $row[] = $date > 0 ? format_date($date) : $date;

    $row[] = drupal_render($item['actions']);
    $rows[] = $row;
  }
  return theme('table', array('header' => $form['#header'], 'rows' => $rows));
}


/**
 * Display table with hosts.
 */
function theme_yandex_webmaster_top_table($variables) {
  drupal_add_css(drupal_get_path('module', 'yandex_webmaster') . '/yandex_webmaster.css');
  $form = $variables['form'];
  $rows = array();
  foreach (element_children($form) as $key) {
    $item = $form[$key];
    $rows[] = array(
      drupal_render($item['rank']),
      drupal_render($item['query']),
      drupal_render($item['count']),
      drupal_render($item['position']),
      drupal_render($item['custom']),
    );
  }
  return theme('table', array('header' => $form['#header'], 'rows' => $rows));
}


/**
 * Display table of excluded pages.
 */
function theme_yandex_webmaster_excluded_table($variables) {
  drupal_add_css(drupal_get_path('module', 'yandex_webmaster') . '/yandex_webmaster.css');
  $form = $variables['form'];
  $rows = array();
  $host = variable_get('yandex_webmaster_basic_host_id', 0);
  foreach (element_children($form) as $key) {
    $item = $form[$key];
    $code = drupal_render($item['code']);
    $severity = drupal_render($item['severity']);
    $rows[$key] = array(
      'data' => array(
        $severity,
        yandex_webmaster_code_description($code),
        drupal_render($item['count']),
        l(t('View'), 'http://webmaster.yandex.com/site/errors/types.xml?host=' . $host . '&code=' . $code, array(
          'attributes' => array(
            'target' => '_blank',
            'title' => t('Open in new window'),
          ),
        )),
      ),
      'class' => array(drupal_strtolower($severity)),
    );
  }
  return theme('table', array('header' => $form['#header'], 'rows' => $rows));
}

/**
 * Display one column table.
 */
function theme_yandex_webmaster_list_table($variables) {
  drupal_add_css(drupal_get_path('module', 'yandex_webmaster') . '/yandex_webmaster.css');
  $form = $variables['form'];
  $rows = array();
  foreach (element_children($form) as $key) {
    $item = $form[$key];
    $rows[] = array(
      drupal_render($item['value']),
    );
  }
  return theme('table', array('header' => $form['#header'], 'rows' => $rows));
}
