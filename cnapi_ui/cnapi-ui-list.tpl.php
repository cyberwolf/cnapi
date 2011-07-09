<?php if ($total == 0) : ?>
  <p><?php print $no_results ?></p>
<?php else : ?>
  <p><?php print $result_info ?></p>
  
  <p><?php print t('Sort by') ?> : <?php print $sort_links ?></p>
  
  <p><?php print $rss ?></p>
  
  <ul>
  <?php foreach ($objects as $object) : ?>
    <li><?php print $object ?></li>
  <?php endforeach; ?>
  </ul>
<?php endif; ?>