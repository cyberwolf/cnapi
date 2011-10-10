<ul class="filter-level-<?php print (string)($level++) ?>">
<?php foreach ($options as $i => $option) : ?>
  <li class="filter-option-<?php print (string)$i ?><?php print $option['active'] ? ' active' : '' ?>">
    <?php print theme('cnapi_browse_filter_option', array('option' => $option)); ?>
    <?php if (!empty($option['children'])) : ?>
      <?php print theme('cnapi_browse_filter_options', array('options' => $option['children'], 'level' => $level)); ?>
    <?php endif; ?>
  </li>
<?php endforeach; ?>
</ul>