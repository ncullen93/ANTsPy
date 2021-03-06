
 

__all__ = ['reorient_image',
           'center_of_mass']

import os
import numpy as np
from tempfile import mktemp

from . import apply_transforms
from .. import lib


_reorient_image_dict = {
    'float': {
        2: lib.reorientImageF2,
        3: lib.reorientImageF3,
        4: lib.reorientImageF4
    } 
}

_center_of_mass_dict = {
    'float': {
        2: lib.centerOfMassF2,
        3: lib.centerOfMassF3,
        4: lib.centerOfMassF4
    }
}


def reorient_image(img, axis1, axis2=None, doreflection=0, doscale=0, txfn=None):
    """
    Example
    -------
    >>> import ants
    >>> img = ants.image_read(ants.get_ants_data('r16'))
    >>> ants.reorient_image(fi, (1,0))
    """
    inpixeltype = img.pixeltype
    if img.pixeltype != 'float':
        img = img.clone('float')

    axis_was_none = False
    if axis2 is None:
        axis_was_none = True
        axis2 = [0]*img.dimension

    axis1 = np.array(axis1)
    axis2 = np.array(axis2)

    axis1 = axis1 / np.sqrt(np.sum(axis1*axis1)) * (-1)
    axis1 = axis1.astype('int')

    if not axis_was_none:
        axis2 = axis2 / np.sqrt(np.sum(axis2*axis2)) * (-1)
        axis2 = axis2.astype('int')
    else:
        axis2 = np.array([0]*img.dimension).astype('int')

    if txfn is None:
        txfn = mktemp(suffix='.mat')

    if isinstance(doreflection, tuple):
        doreflection = list(doreflection)
    if not isinstance(doreflection, list):
        doreflection = [doreflection]

    if isinstance(doscale, tuple):
        doscale = list(doscale)
    if not isinstance(doscale, list):
        doscale = [doscale]

    if len(doreflection) == 1:
        doreflection = [doreflection[0]]*img.dimension
    if len(doscale) == 1:
        doscale = [doscale[0]]*img.dimension

    reorient_image_fn = _reorient_image_dict[img.pixeltype][img.dimension]
    reorient_image_fn(img._img, txfn, axis1.tolist(), axis2.tolist(), doreflection, doscale)
    img2 = apply_transforms(img, img, transformlist=[txfn])

    if img.pixeltype != inpixeltype:
        img2 = img2.clone(inpixeltype)

    return {'reoimg':img2,
            'txfn':txfn}


def center_of_mass(img):
    """
    Calculate the Center of Mass for an ANTsImage
    """
    if img.pixeltype != 'float':
        img = img.clone('float')

    center_of_mass_fn = _center_of_mass_dict[img.pixeltype][img.dimension]
    com = center_of_mass_fn(img._img)

    return tuple(com)



