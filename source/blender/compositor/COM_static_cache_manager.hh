/* SPDX-FileCopyrightText: 2023 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

#include "COM_bokeh_kernel.hh"
#include "COM_cached_image.hh"
#include "COM_cached_mask.hh"
#include "COM_cached_shader.hh"
#include "COM_deriche_gaussian_coefficients.hh"
#include "COM_distortion_grid.hh"
#include "COM_fog_glow_kernel.hh"
#include "COM_image_coordinates.hh"
#include "COM_keying_screen.hh"
#include "COM_morphological_distance_feather_weights.hh"
#include "COM_ocio_color_space_conversion_shader.hh"
#include "COM_smaa_precomputed_textures.hh"
#include "COM_symmetric_blur_weights.hh"
#include "COM_symmetric_separable_blur_weights.hh"
#include "COM_van_vliet_gaussian_coefficients.hh"

namespace blender::compositor {

/* -------------------------------------------------------------------------------------------------
 * Static Cache Manager
 *
 * A static cache manager is a collection of cached resources that can be retrieved when needed and
 * created if not already available. In particular, each cached resource type has its own instance
 * of a container derived from the CachedResourceContainer type in the class. All instances of that
 * cached resource type are stored and tracked in the container. See the CachedResource and
 * CachedResourceContainer classes for more information.
 *
 * The manager deletes the cached resources that are no longer needed. A cached resource is said to
 * be not needed when it was not used in the previous evaluation. This is done through the
 * following mechanism:
 *
 * - Before every evaluation, do the following:
 *     1. All resources whose CachedResource::needed flag is false are deleted.
 *     2. The CachedResource::needed flag of all remaining resources is set to false.
 * - During evaluation, when retrieving any cached resource, set its CachedResource::needed flag to
 *   true.
 *
 * In effect, any resource that was used in the previous evaluation but was not used in the current
 * evaluation will be deleted before the next evaluation. This mechanism is implemented in the
 * reset() method of the class, which should be called before every evaluation. The reset for the
 * next evaluation can be skipped by calling the skip_next_reset() method, see its description for
 * more information. */
class StaticCacheManager {
 public:
  SymmetricBlurWeightsContainer symmetric_blur_weights;
  SymmetricSeparableBlurWeightsContainer symmetric_separable_blur_weights;
  MorphologicalDistanceFeatherWeightsContainer morphological_distance_feather_weights;
  CachedMaskContainer cached_masks;
  SMAAPrecomputedTexturesContainer smaa_precomputed_textures;
  OCIOColorSpaceConversionShaderContainer ocio_color_space_conversion_shaders;
  DistortionGridContainer distortion_grids;
  KeyingScreenContainer keying_screens;
  CachedShaderContainer cached_shaders;
  BokehKernelContainer bokeh_kernels;
  CachedImageContainer cached_images;
  DericheGaussianCoefficientsContainer deriche_gaussian_coefficients;
  VanVlietGaussianCoefficientsContainer van_vliet_gaussian_coefficients;
  FogGlowKernelContainer fog_glow_kernels;
  ImageCoordinatesContainer image_coordinates;

 private:
  /* The cache manager should skip the next reset. See the skip_next_reset() method for more
   * information. */
  bool should_skip_next_reset_ = false;

 public:
  /* Reset the cache manager by deleting the cached resources that are no longer needed because
   * they weren't used in the last evaluation and prepare the remaining cached resources to track
   * their needed status in the next evaluation. See the class description for more information.
   * This should be called before every evaluation. */
  void reset();

  /* Specifies that the cache manager should skip the next reset. This is useful for instance when
   * the evaluation gets canceled before it was fully done, in that case, we wouldn't want to
   * invalidate the cache because not all operations that use cached resources got the chance to
   * mark their used resources as still in use. So we wait until a full evaluation happen before we
   * decide that some resources are no longer needed. */
  void skip_next_reset();
};

}  // namespace blender::compositor
