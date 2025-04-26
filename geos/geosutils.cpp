#include "geosutils.hpp"

#include "feature.hpp"

#include <vector>

#include <iostream>

static void geos_message_handler(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    // fprintf(stderr, "GEOS Error %d: ", errcode);
    // vfprintf(stderr, fmt, args);
    va_end(args);
}

std::vector<float> point_line_buffer(const Feature &f, int buffer_size)
{
    initGEOS(geos_message_handler, geos_message_handler);

    std::vector<float> buffer;

    switch (f.gtype)
    {

    case POINT:
    {
        int size = f.coords[0].size() / 2;

        GEOSCoordSequence *seq = GEOSCoordSeq_create(size, 2);

        for (int i = 0; i < size; i++)
        {
            GEOSCoordSeq_setX(seq, i, f.coords[0][2 * i]);
            GEOSCoordSeq_setY(seq, i, f.coords[0][2 * i + 1]);
        }

        GEOSGeometry *geom = GEOSGeom_createPoint(seq);
        GEOSGeometry *buffer_geom = GEOSBuffer(geom, buffer_size, 8);

        GEOSWKTWriter * wkt_writer = GEOSWKTWriter_create();

        

        const GEOSGeometry *ring = GEOSGetExteriorRing(GEOSGetGeometryN(buffer_geom, 0));

        // std::cout << GEOSWKTWriter_write(wkt_writer,ring) << std::endl;

        const GEOSCoordSequence *ring_seq = GEOSGeom_getCoordSeq(ring);

        int num_points = GEOSGeomGetNumPoints(ring);

        for (int i = 0; i < num_points; i++)
        {

            double tx;
            double ty;

            GEOSCoordSeq_getX(ring_seq, i, &tx);
            GEOSCoordSeq_getY(ring_seq, i, &ty);
            buffer.push_back(static_cast<float>(tx));
            buffer.push_back(static_cast<float>(ty));
        }

        GEOSGeom_destroy(geom);
        GEOSGeom_destroy(buffer_geom);
    }

    break;

    default:

    {
        int size = f.coords[0].size() / 2;

        GEOSCoordSequence *seq = GEOSCoordSeq_create(size, 2);

        // const std::vector<int> * tmp = &(f.coords[0]);

        for (int i = 0; i < size; i++)
        {
            GEOSCoordSeq_setX(seq, i, f.coords[0][2 * i]);
            GEOSCoordSeq_setY(seq, i, f.coords[0][2 * i + 1]);
        }

        GEOSGeometry *geom = GEOSGeom_createLineString(seq);
        GEOSGeometry *buffer_geom = GEOSBuffer(geom, buffer_size, 8);

        const GEOSGeometry *ring = GEOSGetExteriorRing(GEOSGetGeometryN(buffer_geom, 0));

        const GEOSCoordSequence *ring_seq = GEOSGeom_getCoordSeq(ring);

        int num_points = GEOSGeomGetNumPoints(ring);

        for (int i = 0; i < num_points; i++)
        {

            double tx;
            double ty;

            GEOSCoordSeq_getX(ring_seq, i, &tx);
            GEOSCoordSeq_getY(ring_seq, i, &ty);
            buffer.push_back(static_cast<float>(tx));
            buffer.push_back(static_cast<float>(ty));
        }

        GEOSGeom_destroy(geom);
        GEOSGeom_destroy(buffer_geom);
    }
    break;
    }

    finishGEOS();

    return buffer;
}