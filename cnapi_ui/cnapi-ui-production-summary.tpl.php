<div class="production-summary">
  <h2><?php print $title ?></h2>

  <?php if ($thumbnail) : ?>
    <?php print $thumbnail ?>
  <?php endif; ?>

  <div class="short-description"><?php print $short_description ?></div>

  <dl>
    <?php if ($who) : ?>
      <dt class="who"><?php print t('Who')?></dt>
      <dd class="who"><?php print $who ?><dd>
    <?php endif; ?>
  </dl>
</div>