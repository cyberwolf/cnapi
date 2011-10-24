<?php if ($total == 0) : ?>
  <div class="result-title"><?php print $no_results ?></div>
<?php else : ?>
  <div class="result-title"><?php print $result_info ?></div>
  
  <div class="result-controls">
    
    <p><?php print $result_counter ?></p>

    <p><?php print t('Sort by') ?>: <?php print $sort_links ?></p>

    <p><?php print $rss ?></p>
    
  </div>

  <ul>
    <?php foreach ($objects as $object) : ?>
      <li><?php print $object ?></li>
    <?php endforeach; ?>
  </ul>
  
<?php endif; ?>

<?php print $pager ?>