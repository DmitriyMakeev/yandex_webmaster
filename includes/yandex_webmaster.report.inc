<?php

/**
 * @file
 * Callback functions for report pages for the Yandex.Webmaster module.
 *
 * @ingroup yandex_webmaster
 */

/**
 * Load idna_convert class for correct work with domain-names in punycode.
 */
require_once('idna_convert.class.php');

/**
 * Display summary statistics for the Yandex.Webmaster module.
 *
 * @ingroup yandex_webmaster
 */
function yandex_webmaster_report_form($form, $form_state) {
  drupal_set_title(t('Yandex.Webmaster summary'));
  drupal_add_css(drupal_get_path('module', 'yandex_webmaster') . '/yandex_webmaster.css');
  $form = array();
  // Get host list.
  $hostlist = yandex_webmaster_get_hosts();
  yandex_webmaster_update_stats($hostlist);
  // Get selected host id.
  $id = isset($hostlist[(int) arg(3)]) ? (int) arg(3) : 0;
  $form['branding'] = yandex_webmaster_branding_item();
  $form['selector'] = yandex_webmaster_selector_item($hostlist);

  // Get statistics.
  $header = array(
    'timestamp' => array('data' => t('Date'), 'field' => 's.timestamp'),
    'tcy' => array('data' => t('TCY'), 'field' => 's.tcy'),
    'url_count' => array('data' => t('URL count'), 'field' => 's.url_count'),
    'index_count' => array('data' => t('Indexed pages'), 'field' => 's.index_count'),
    'links_count' => array('data' => t('External links'), 'field' => 's.links_count'),
    'url_errors' => array('data' => t('URL errors'), 'field' => 's.url_errors'),
  );
  $stats = db_select('yandex_webmaster_stats', 's')
    ->extend('PagerDefault')
    ->extend('TableSort')
    ->fields('s')
    ->condition('s.host_id', (int) arg(3))
    ->limit(100)
    ->orderByHeader($header)
    ->execute();

  $is_chart = module_exists('chart');
  if ($is_chart) {
    $chart = array(
      '#chart_id' => 'statistics_chart',
      '#title' => t('Statistics'),
      '#type' => CHART_TYPE_LINE,
      '#size' => chart_size(960, 250),
      '#grid_lines' => chart_grid_lines(25, 9.5, 1, 3),
      /* '#mixed_axis_labels' => array(
        CHART_AXIS_Y_LEFT => array(0 => chart_mixed_axis_range_label(0, $max)),
      ), */
      '#adjust_resolution' => TRUE,
      '#data_colors' => array(
        'FF8000',
        '803000',
        '0090C0',
        '30A020',
        'FF0000',
      ),
      '#legends' => array(
        t('TCY'),
        t('URL count'),
        t('Indexed pages'),
        t('External links'),
        t('URL errors'),
      ),
      '#line_styles' => array(2, 2, 2, 2, 2),
    );
  }
  $options = array();
  foreach ($stats as $aid => $stat) {
    $options[$aid] = array(
      'timestamp' => format_date($stat->timestamp),
      'tcy' => number_format($stat->tcy, 0, '', ' '),
      'url_count' => number_format($stat->url_count, 0, '', ' '),
      'url_errors' => number_format($stat->url_errors, 0, '', ' '),
      'index_count' => number_format($stat->index_count, 0, '', ' '),
      'links_count' => number_format($stat->links_count, 0, '', ' '),
    );
    if ($is_chart) {
      $chart['#labels'][] = date('d.m.Y', $stat->timestamp);
      $chart['#data']['tcy'][] = $stat->tcy;
      $chart['#data']['url_count'][] = $stat->url_count;
      $chart['#data']['index_count'][] = $stat->index_count;
      $chart['#data']['links_count'][] = $stat->links_count;
      $chart['#data']['url_errors'][] = $stat->url_errors;
    }
  }
  $form['stat'] = array(
    '#theme' => 'table',
    '#type' => 'tableselect',
    '#header' => $header,
    '#rows' => $options,
    '#options' => $options,
    '#empty' => t('No statistics available.'),
  );

  if ($is_chart) {
    $form['chart'] = array(
      '#markup' => theme('chart', array('chart' => $chart)),
    );
  }

  return $form;
}

/**
 * Callback function for yandex_webmaster_report_form().
 *
 * @ingroup yandex_webmaster
 */
function yandex_webmaster_report_form_submit($form, &$form_state) {
  if ($form_state['values']['selector']['save'] == 1) {
    variable_set('yandex_webmaster_basic_host_id', (int) $form_state['values']['selector']['options']);
    menu_rebuild();
  }
  drupal_goto('admin/reports/yandex_webmaster/' . (int) $form_state['values']['selector']['options']);
}

/**
 * Display top clicks and shows report for the Yandex.Webmaster module.
 *
 * @ingroup yandex_webmaster
 */
function yandex_webmaster_report_top_form($form, $form_state) {
  drupal_set_title(t('Yandex.Webmaster top queries'));
  drupal_add_css(drupal_get_path('module', 'yandex_webmaster') . '/yandex_webmaster.css');
  drupal_add_js(drupal_get_path('module', 'yandex_webmaster') . '/yandex_webmaster.js');
  $form = array();
  // Get host list.
  $hostlist = yandex_webmaster_get_hosts();
  // Get selected host id.
  $id = isset($hostlist[(int) arg(3)]) ? (int) arg(3) : 0;
  // Get statistics.
  $stats = yandex_webmaster_get_top_clicks($id);
  if (isset($stats['error'])) {
    drupal_set_message(t('An error occured while getting clicks statistics. @error !link.', array(
      '@error' => filter_xss($stats['error']),
      '!link' => l(t('Yandex.Webmaster settings page'), 'admin/config/services/yandex_webmaster'),
    )), 'error');
    return $form;
  }
  $form['branding'] = yandex_webmaster_branding_item();
  $form['selector'] = yandex_webmaster_selector_item($hostlist);

  // Empty statistics.
  if ($stats['total-clicks-count'] == 0) {
    $form['summary'] = array(
      '#type' => 'item',
      '#markup' => t('Summary click count for last seven days') . ': '
      . $stats['total-clicks-count'] . '<br />'
      . t('Summary shows count for last seven days') . ': '
      . $stats['total-shows-count'],
    );
    return $form;
  }

  $form['procent'] = array(
    '#type' => 'item',
    '#markup' => t('Percentage of shows count to the number of clicks') . ': ' . $stats['shows-clicks-procent'] . '%',
  );

  // Display top clicks list.
  $form['clicks_summary'] = array(
    '#type' => 'item',
    '#markup' => t('Click count for last seven days') . ': '
      . $stats['total-clicks-count'] . '<br />'
      . t('Procent of popular query clicks') . ': '
      . $stats['top-clicks-percent'] . '%',
  );
  $form['clicks'] = array(
    '#type' => 'fieldset',
    '#title' => t('Top clicks for last seven days'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $clicks = array();
  foreach ($stats['clicks'] as $info) {
    $clicks[] = array(
      $info['clicks-top-rank'],
      $info['query'],
      $info['count'],
      $info['position'],
      $info['is-custom'],
    );
  }
  $form['clicks']['table'] = array(
    '#type' => 'markup',
    '#markup' => theme('table', array(
      'header' => array(
        '#',
        t('Query'),
        t('Click count'),
        t('Position'),
        t('User query'),
      ),
      'rows' => $clicks,
    ))
  );

  // Display top shows list.
  $form['shows_summary'] = array(
    '#type' => 'item',
    '#markup' => t('Shows count for last seven days') . ': '
      . $stats['total-shows-count'] . '<br />'
      . t('Procent of popular query shows') . ': '
      . $stats['top-shows-percent'] . '%',
  );
  $form['shows'] = array(
    '#type' => 'fieldset',
    '#title' => t('Top shows for last seven days'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $shows = array();
  foreach ($stats['shows'] as $info) {
    $shows[] = array(
      $info['shows-top-rank'],
      $info['query'],
      $info['count'],
      $info['position'],
      $info['is-custom'],
    );
  }
  $form['shows']['table'] = array(
    '#type' => 'markup',
    '#markup' => theme('table', array(
      'header' => array(
        '#',
        t('Query'),
        t('Shows count'),
        t('Position'),
        t('User query'),
      ),
      'rows' => $shows,
    ))
  );
  return $form;
}

/**
 * Callback function for yandex_webmaster_report_top_form().
 *
 * @ingroup yandex_webmaster
 */
function yandex_webmaster_report_top_form_submit($form, &$form_state) {
  if ($form_state['values']['selector']['save'] == 1) {
    variable_set('yandex_webmaster_basic_host_id', (int) $form_state['values']['selector']['options']);
    menu_rebuild();
  }
  drupal_goto('admin/reports/yandex_webmaster/' . (int) $form_state['values']['selector']['options'] . '/top');
}

/**
 * Display last indexed pages report for the Yandex.Webmaster module.
 *
 * @ingroup yandex_webmaster
 */
function yandex_webmaster_report_last_form($form, $form_state) {
  drupal_set_title(t('Yandex.Webmaster last indexed pages'));
  drupal_add_css(drupal_get_path('module', 'yandex_webmaster') . '/yandex_webmaster.css');
  $form = array();
  // Get host list.
  $hostlist = yandex_webmaster_get_hosts();
  // Get selected host id.
  $id = isset($hostlist[(int) arg(3)]) ? (int) arg(3) : 0;
  // Get statistics.
  $stats = yandex_webmaster_get_last_index($id);
  if (isset($stats['error'])) {
    drupal_set_message(t('An error occured while getting last indexed pages statistics. @error !link.', array(
      '@error' => filter_xss($stats['error']),
      '!link' => l(t('Yandex.Webmaster settings page'), 'admin/config/services/yandex_webmaster'),
    )), 'error');
    return $form;
  }
  $form['branding'] = yandex_webmaster_branding_item();
  $form['selector'] = yandex_webmaster_selector_item($hostlist);

  $host = variable_get('yandex_webmaster_basic_host_id', 0);
  $form['index_summary'] = array(
    '#type' => 'item',
    '#markup' => t('Summary count of indexed pages') . ': '
      . $stats['index-count']
      . '<br />'
      . t('Indexed pages for last seven days') . ': '
      . count($stats['url']),
  );
  $form['view_all'] = array(
    '#type' => 'item',
    '#markup' => l(t('View all indexed pages'), yandex_webmaster_domain_name() . 'site/indexed-pages.xml?host=' . $host . '&path=*', array(
      'attributes' => array(
        'target' => '_blank',
        'title' => t('Open in new window'),
      ),
    )),
  );

  // Display last indexed pages table.
  if (count($stats['url'])) {
    $list = array();
    $counter = 0;
    foreach ($stats['url'] as $url) {
      ++$counter;
      $list[] = array(
        $counter,
        urldecode($url),
      );
    }
    $form['table'] = array(
      '#title' => t('Indexed pages for last seven days'),
      '#type' => 'markup',
      '#markup' => theme('table', array(
        'header' => array(
          '#',
          t('Address'),
        ),
        'rows' => $list,
      ))
    );
  }

  return $form;
}

/**
 * Callback function for yandex_webmaster_report_last_form().
 *
 * @ingroup yandex_webmaster
 */
function yandex_webmaster_report_last_form_submit($form, &$form_state) {
  if ($form_state['values']['selector']['save'] == 1) {
    variable_set('yandex_webmaster_basic_host_id', (int) $form_state['values']['selector']['options']);
    menu_rebuild();
  }
  drupal_goto('admin/reports/yandex_webmaster/' . (int) $form_state['values']['selector']['options'] . '/last');
}

/**
 * Display excluded from the index pages report for the Yandex.Webmaster module.
 *
 * @ingroup yandex_webmaster
 */
function yandex_webmaster_report_excluded_form($form, $form_state) {
  drupal_set_title(t('Yandex.Webmaster excluded pages'));
  drupal_add_css(drupal_get_path('module', 'yandex_webmaster') . '/yandex_webmaster.css');
  drupal_add_js(drupal_get_path('module', 'yandex_webmaster') . '/yandex_webmaster.js');
  $form = array();
  // Get host list.
  $hostlist = yandex_webmaster_get_hosts();
  // Get selected host id.
  $id = isset($hostlist[(int) arg(3)]) ? (int) arg(3) : 0;
  // Get statistics.
  $stats = yandex_webmaster_get_excluded_pages($id);
  if (isset($stats['error'])) {
    drupal_set_message(t('An error occured while getting excluded pages statistics. @error !link.', array(
      '@error' => filter_xss($stats['error']),
      '!link' => l(t('Yandex.Webmaster settings page'), 'admin/config/services/yandex_webmaster'),
    )), 'error');
    return $form;
  }
  $form['branding'] = yandex_webmaster_branding_item();
  $form['selector'] = yandex_webmaster_selector_item($hostlist);

  $form['excluded_summary'] = array(
    '#type' => 'item',
    '#markup' => t('Summary count of excluded pages') . ': ' . $stats['url-errors'],
  );

  // Display excluded pages table.
  if (count($stats['url'])) {
    $list = array();
    $host = (int) variable_get('yandex_webmaster_basic_host_id', 0);
    foreach ($stats['url'] as $info) {
      $list[] = array(
        'data' => array(
          $info['severity'],
          yandex_webmaster_code_description($info['code']),
          $info['count'],
          l(t('View'), yandex_webmaster_domain_name() . 'site/errors/types.xml?host=' . $host . '&code=' . $info['code'], array(
            'attributes' => array(
              'target' => '_blank',
              'title' => t('Open in new window'),
            ),
          )),
        ),
        'class' => array(drupal_strtolower($info['severity'])),
      );
    }
    $form['table'] = array(
      '#title' => t('External links for last seven days'),
      '#type' => 'markup',
      '#markup' => theme('table', array(
        'header' => array(
          t('Severity'),
          t('Error type'),
          t('Count'),
          t('Actions'),
        ),
        'rows' => $list,
      ))
    );
  }

  return $form;
}

/**
 * Callback function for yandex_webmaster_report_excluded_form().
 *
 * @ingroup yandex_webmaster
 */
function yandex_webmaster_report_excluded_form_submit($form, &$form_state) {
  if ($form_state['values']['selector']['save'] == 1) {
    variable_set('yandex_webmaster_basic_host_id', (int) $form_state['values']['selector']['options']);
    menu_rebuild();
  }
  drupal_goto('admin/reports/yandex_webmaster/' . (int) $form_state['values']['selector']['options'] . '/errors');
}

/**
 * Display external links report for the Yandex.Webmaster module.
 *
 * @ingroup yandex_webmaster
 */
function yandex_webmaster_report_links_form($form, $form_state) {
  drupal_set_title(t('Yandex.Webmaster external links'));
  drupal_add_css(drupal_get_path('module', 'yandex_webmaster') . '/yandex_webmaster.css');
  $form = array();
  // Get host list.
  $hostlist = yandex_webmaster_get_hosts();
  // Get selected host id.
  $id = isset($hostlist[(int) arg(3)]) ? (int) arg(3) : 0;
  // Get statistics.
  $stats = yandex_webmaster_get_external_links($id);
  if (isset($stats['error'])) {
    drupal_set_message(t('An error occured while getting external links statistics. @error !link.', array(
      '@error' => filter_xss($stats['error']),
      '!link' => l(t('Yandex.Webmaster settings page'), 'admin/config/services/yandex_webmaster'),
    )), 'error');
    return $form;
  }
  $form['branding'] = yandex_webmaster_branding_item();
  $form['selector'] = yandex_webmaster_selector_item($hostlist);

  $host = variable_get('yandex_webmaster_basic_host_id', 0);
  $form['index_summary'] = array(
    '#type' => 'item',
    '#markup' => t('Summary count of external links') . ': '
      . $stats['links-count']
      . '<br />'
      . t('External links for last seven days') . ': '
      . count($stats['url']),
  );
  $form['view_all'] = array(
    '#type' => 'item',
    '#markup' => l(t('View all external links'), 'http://webmaster.yandex.com/site/indexed/links.xml?host=' . $host . '&path=*', array(
      'attributes' => array(
        'target' => '_blank',
        'title' => t('Open in new window'),
      ),
    )),
  );

  // Display last indexed pages table.
  if (count($stats['url'])) {
    $list = array();
    $counter = 0;
    foreach ($stats['url'] as $url) {
      ++$counter;
      $list[] = array(
        $counter,
        urldecode($url),
      );
    }
    $form['table'] = array(
      '#title' => t('External links for last seven days'),
      '#type' => 'markup',
      '#markup' => theme('table', array(
        'header' => array(
          '#',
          t('Address'),
        ),
        'rows' => $list,
      ))
    );
  }

  return $form;
}

/**
 * Callback function for yandex_webmaster_report_links_form().
 *
 * @ingroup yandex_webmaster
 */
function yandex_webmaster_report_links_form_submit($form, &$form_state) {
  if ($form_state['values']['selector']['save'] == 1) {
    variable_set('yandex_webmaster_basic_host_id', (int) $form_state['values']['selector']['options']);
    menu_rebuild();
  }
  drupal_goto('admin/reports/yandex_webmaster/' . (int) $form_state['values']['selector']['options'] . '/links');
}
