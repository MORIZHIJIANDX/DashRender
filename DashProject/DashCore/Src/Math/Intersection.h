#pragma once

#include "MathType.h"

namespace Dash
{
	namespace FMath
	{
		bool RayTriangleIntersection(const FRay& r, const FVector3f& v0, const FVector3f& v1, const FVector3f& v2, Scalar& u, Scalar& v, Scalar& t) noexcept;
		bool RayTriangleIntersection(const FRay& r, const FVector3f& v0, const FVector3f& v1, const FVector3f& v2) noexcept;

		bool RayBoundingBoxIntersection(const FRay& r, const FBoundingBox& b, Scalar& t0, Scalar& t1) noexcept;

		bool RaySphereIntersection(const FRay& r, const FVector3f& center, Scalar radius, Scalar& t0, Scalar& t1) noexcept;

		bool RayPlaneIntersection(const FRay& r, const FVector3f& normal, const FVector3f& p, Scalar& t) noexcept;

	
		FORCEINLINE bool RayTriangleIntersection(const FRay& r, const FVector3f& v0, const FVector3f& v1, const FVector3f& v2) noexcept
		{
			Scalar u, v, t;
			return RayTriangleIntersection(r, v0, v1, v2, u, v, t);
		}

		FORCEINLINE bool RayTriangleIntersection(const FRay& r, const FVector3f& v0, const FVector3f& v1, const FVector3f& v2,
			Scalar& u, Scalar& v, Scalar& t) noexcept
		{
			FVector3f v0v1 = v1 - v0;
			FVector3f v0v2 = v2 - v0;
			FVector3f pvec = Cross(r.Direction, v0v2);
			Scalar det = Dot(v0v1, pvec);

			//FRay triangle parallel
			if (Abs(det) < TScalarTraits<Scalar>::Epsilon())
				return false;

			Scalar invDet = Scalar{ 1 } / det;

			FVector3f tvec = r.Origin - v0;
			u = Dot(tvec, pvec) * invDet;
			if (u < 0 || u > 1) return false;

			FVector3f qvec = Cross(tvec, v0v1);
			v = Dot(r.Direction, qvec) * invDet;
			if (v < 0 || v > 1) return false;

			t = Dot(v0v2, qvec) * invDet;

			return true;
		}

		FORCEINLINE bool RayBoundingBoxIntersection(const FRay& r, const FBoundingBox& b, Scalar& t0, Scalar& t1) noexcept
		{
			Scalar tMin = 0, tMax = r.TMax;
			for (std::size_t i = 0; i < 3; i++)
			{
				Scalar invRayDir = Scalar{ 1 } / r.Direction[i];
				Scalar tNear = (b.Lower[i] - r.Origin[i]) * invRayDir;
				Scalar tFar = (b.Upper[i] - r.Origin[i]) * invRayDir;

				if (tNear > tFar)
					Swap(tNear, tFar);

				tMin = tNear > tMin ? tNear : tMin;
				tMax = tFar < tMax ? tFar : tMax;

				if (tMin > tMax)
					return false;
			}

			t0 = tMin;
			t1 = tMax;

			return true;
		}

		FORCEINLINE bool RaySphereIntersection(const FRay& r, const FVector3f& center, Scalar radius, Scalar& t0, Scalar& t1) noexcept
		{
			FVector3f oc = r.Origin - center;
			Scalar a = Dot(r.Direction, r.Direction);
			Scalar b = Scalar{ 2} * Dot(oc, r.Direction);
			Scalar c = Dot(oc, oc) - radius * radius;

			Scalar discrim = b * b - Scalar{ 4 } * a * c;

			if (discrim >= Scalar{ 0 })
			{
				Scalar sqrtDiscrim = FMath::Sqrt(discrim);
				Scalar q = (b >= Scalar{ 0 }) ? (Scalar(-0.5) * (b + sqrtDiscrim)) : (Scalar(-0.5) * (b - sqrtDiscrim));

				t0 = q / a;
				t1 = c / q;

				if (t1 < t0)
					FMath::Swap(t0, t1);
			}
			else
			{
				FVector3f dir = Normalize(r.Direction);
				FVector3f deltap = center - r.Origin;
				Scalar ddp = Dot(dir, deltap);
				Scalar deltapdot = Dot(deltap, deltap);

				FVector3f remedyTerm = deltap - ddp * dir;
				discrim = radius * radius - Dot(remedyTerm, remedyTerm);

				if (discrim >= 0)
				{
					Scalar sqrtDiscrim = FMath::Sqrt(discrim);

					Scalar q = (ddp >= Scalar{ 0 }) ? (ddp + sqrtDiscrim) : (ddp - sqrtDiscrim);

					t0 = q;
					t1 = (deltapdot - radius * radius) / q;

					if (t1 < t0)
						FMath::Swap(t0, t1);
				}
				else
				{
					return false;
				}
			}

			return true;
		}

		FORCEINLINE bool RayPlaneIntersection(const FRay& r, const FVector3f& normal, const FVector3f& p, Scalar& t) noexcept
		{
			Scalar denominator = Dot(normal, r.Direction);

			if (Abs(denominator) <= TScalarTraits<Scalar>::Epsilon())
				return false;

			Scalar numerator = Dot((p - r.Origin), normal);

			t = numerator / denominator;

			return true;
		}

	}
}