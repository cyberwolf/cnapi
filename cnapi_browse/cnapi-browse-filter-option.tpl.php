<?php if ($active) : ?>
  <?php print $name ?>
<?php else : ?>
  <?php print $link ?>
<?php endif; ?>

<?php if (isset($total)) : ?><span class="total">(<?php print $total ?>)</span><?php endif; ?>

<?php if ($active) : ?>
  <?php print $link_remove ?>
<?php endif; ?>